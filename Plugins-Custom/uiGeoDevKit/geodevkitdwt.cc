/*+
________________________________________________________________________

 GeoDevDWT1D / GeoDevDWTCompare 实现

 变换链路（本插件 P1 的核心算法）：
   1. 输入信号长度 n 不是 2 的幂 -> 居中零补齐到 padsz_ = 2^ceil(log2 n)。
      （内核 ::DWT 的 FilterWT1D::transform1Dt 按 2 的幂循环 nn>>=1，
        非 2 幂会漏样点；src/Algo/wavelettrans.cc:512 isPossible 也这么判定。）
   2. ::DWT::run(false) 正向全分解到 1 个近似系数 + 各级细节系数。
      一维全分解后的系数布局（padsz_ = 2^L）：
        coeff[0]        = cA_L
        coeff[1]        = cD_L
        coeff[2..3]     = cD_{L-1}
        ...
        coeff[2^(L-1)..padsz_-1] = cD_1
   3. 取某级分量：把该子带以外的系数全部置零，再 ::DWT::setDir(false)
      做全逆变换，得到该分量在原网格上的波形（长度 padsz_），
      最后取前 n 个样点返回 —— 这就是「同网格 MRA 分量」。
   4. 小波域属性直接从系数算：
        近似能量   = cA_level 系数平方和
        细节能量   = cD_level 系数平方和
        相对细节能量 = sum(cD_1..cD_level) / 总能量
        小波熵     = -sum p_i log2(p_i), p_i = |c_i|^2 / 总能量

 不许用 sqrt / atan2 / strcmp（仓库 Lint 规则）；能量与熵都是系数域统计量，
 无需开方。对数用标准 std::log2。
________________________________________________________________________
-*/

#include "geodevkitdwt.h"

#include "arrayndimpl.h"
#include "bufstringset.h"
#include "od_ostream.h"
#include "ptrman.h"
#include "timefun.h"
#include "wavelettrans.h"

#include <cmath>


static int nextPow2( int n )
{
    int p = 1;
    while ( p<n ) p <<= 1;
    return p;
}


void GeoDevDWT1D::getWaveletNames( BufferStringSet& nms )
{
    nms.erase();
    const int nr = (int)WaveletTransform::WaveletTypeDef().size();
    for ( int idx=0; idx<nr; idx++ )
	nms.add( WaveletTransform::toString((WaveletID)idx) );
}


GeoDevDWT1D::WaveletID GeoDevDWT1D::waveletFromIdx( int idx )
{
    const int nr = (int)WaveletTransform::WaveletTypeDef().size();
    if ( idx<0 || idx>=nr ) return WaveletTransform::Daubechies4;
    return (WaveletID)idx;
}


int GeoDevDWT1D::idxFromWavelet( WaveletID wt )
{
    const int nr = (int)WaveletTransform::WaveletTypeDef().size();
    for ( int idx=0; idx<nr; idx++ )
	if ( (WaveletID)idx==wt ) return idx;
    return -1;
}


int GeoDevDWT1D::maxLevel( int signalsz )
{
    if ( signalsz<2 ) return 0;
    const int p = nextPow2( signalsz );
    int lvl = 0;
    for ( int n=p; n>=2; n>>=1 ) lvl++;
    return lvl;
}


GeoDevDWT1D::GeoDevDWT1D( WaveletID wt )
    : wt_(wt)
{}


GeoDevDWT1D::~GeoDevDWT1D()
{
    cleanup();
}


void GeoDevDWT1D::cleanup()
{
    delete [] coeffs_; coeffs_ = nullptr;
    coefsz_ = 0; padsz_ = 0; orgsz_ = 0; maxlev_ = 0;
    ready_ = false;
}


bool GeoDevDWT1D::decompose( const float* sig, int n )
{
    cleanup();
    if ( !sig || n<2 ) return false;

    orgsz_ = n;
    padsz_ = nextPow2( n );
    coefsz_ = padsz_;
    maxlev_ = maxLevel( n );
    coeffs_ = new float[ coefsz_ ];

    // 居中零补齐：原信号放在 [(padsz_-n)/2, ...)，两侧补 0
    Array1DImpl<float> inbuf( padsz_ );
    for ( int idx=0; idx<padsz_; idx++ ) inbuf.set( idx, 0.f );
    const int off = (padsz_-n)/2;
    for ( int idx=0; idx<n; idx++ )
	inbuf.set( off+idx, sig[idx] );

    PtrMan<::DWT> dwt = new ::DWT( wt_ );
    dwt->setInputInfo( inbuf.info() );
    dwt->setInput( inbuf.getData() );
    dwt->setOutput( coeffs_ );
    if ( !dwt->run(false) )
    {
	cleanup();
	return false;
    }

    ready_ = true;
    return true;
}


bool GeoDevDWT1D::subbandRange( int level, bool approx,
				int& start, int& len ) const
{
    if ( !ready_ || level<1 || level>maxlev_ )
	return false;

    // 级号换算：cD_j 位于 coeff[2^(L-j), 2^(L-j+1))，j=1 最细；
    // cA_level 位于 coeff[0, 2^(L-level))。
    const int L = maxlev_;
    if ( approx )
    {
	start = 0;
	len = 1 << (L-level);
    }
    else
    {
	start = 1 << (L-level);
	len = 1 << (L-level);
    }
    if ( start<0 || len<1 || start+len>coefsz_ ) return false;
    return true;
}


bool GeoDevDWT1D::reconstructApprox( int level, float* out, int n ) const
{
    if ( !out || n<1 ) return false;
    int start=0, len=0;
    if ( !subbandRange(level,true,start,len) ) return false;

    Array1DImpl<float> maskbuf( coefsz_ );
    for ( int idx=0; idx<coefsz_; idx++ ) maskbuf.set( idx, 0.f );
    for ( int idx=0; idx<len; idx++ )
	maskbuf.set( start+idx, coeffs_[start+idx] );

    Array1DImpl<float> outbuf( coefsz_ );
    ::DWT dwt( wt_ );
    dwt.setInputInfo( maskbuf.info() );
    dwt.setInput( maskbuf.getData() );
    dwt.setOutput( outbuf.getData() );
    dwt.setDir( false );
    if ( !dwt.run(false) ) return false;

    const int off = (padsz_-orgsz_)/2;
    for ( int idx=0; idx<n && idx+off<coefsz_; idx++ )
	out[idx] = outbuf.get( idx+off );
    return true;
}


bool GeoDevDWT1D::reconstructDetail( int level, float* out, int n ) const
{
    if ( !out || n<1 ) return false;
    int start=0, len=0;
    if ( !subbandRange(level,false,start,len) ) return false;

    Array1DImpl<float> maskbuf( coefsz_ );
    for ( int idx=0; idx<coefsz_; idx++ ) maskbuf.set( idx, 0.f );
    for ( int idx=0; idx<len; idx++ )
	maskbuf.set( start+idx, coeffs_[start+idx] );

    Array1DImpl<float> outbuf( coefsz_ );
    ::DWT dwt( wt_ );
    dwt.setInputInfo( maskbuf.info() );
    dwt.setInput( maskbuf.getData() );
    dwt.setOutput( outbuf.getData() );
    dwt.setDir( false );
    if ( !dwt.run(false) ) return false;

    const int off = (padsz_-orgsz_)/2;
    for ( int idx=0; idx<n && idx+off<coefsz_; idx++ )
	out[idx] = outbuf.get( idx+off );
    return true;
}


double GeoDevDWT1D::approxEnergy( int level ) const
{
    int start=0, len=0;
    if ( !subbandRange(level,true,start,len) ) return 0;
    double e = 0;
    for ( int idx=start; idx<start+len; idx++ )
    {
	const double v = coeffs_[idx];
	e += v*v;
    }
    return e;
}


double GeoDevDWT1D::detailEnergy( int level ) const
{
    int start=0, len=0;
    if ( !subbandRange(level,false,start,len) ) return 0;
    double e = 0;
    for ( int idx=start; idx<start+len; idx++ )
    {
	const double v = coeffs_[idx];
	e += v*v;
    }
    return e;
}


double GeoDevDWT1D::detailEnergyTotal( int level ) const
{
    double e = 0;
    for ( int lvl=1; lvl<=level && lvl<=maxlev_; lvl++ )
	e += detailEnergy( lvl );
    return e;
}


double GeoDevDWT1D::relativeDetailEnergy( int level ) const
{
    const double ae = approxEnergy( level );
    const double de = detailEnergyTotal( level );
    const double tot = ae+de;
    if ( tot<1e-12 ) return 0;
    return de/tot;
}


double GeoDevDWT1D::entropy() const
{
    if ( !ready_ ) return 0;
    double tot = 0;
    for ( int idx=0; idx<coefsz_; idx++ )
    {
	const double v = coeffs_[idx];
	tot += v*v;
    }
    if ( tot<1e-12 ) return 0;

    double h = 0;
    for ( int idx=0; idx<coefsz_; idx++ )
    {
	const double v = coeffs_[idx];
	const double p = (v*v)/tot;
	if ( p>1e-12 )
	    h -= p*std::log2( p );
    }
    return h;
}


bool GeoDevDWT1D::evaluate( WaveletID wt, const float* sig, int n,
			    int level, Metrics& met )
{
    GeoDevDWT1D eng( wt );
    Time::Counter timer; timer.start();
    if ( !eng.decompose(sig,n) ) return false;

    met.energy_ = 0;
    const float* c = eng.coeffs();
    const int sz = eng.coeffSize();
    for ( int idx=0; idx<sz; idx++ )
    {
	const double v = c[idx];
	met.energy_ += v*v;
    }
    const double tot = met.energy_>1e-12 ? met.energy_ : 1;

    // 稀疏度：系数能量低于「平均能量 * 1%」的比例
    const double thr = tot/sz*0.01;
    int nbelow = 0;
    for ( int idx=0; idx<sz; idx++ )
    {
	const double v = c[idx];
	if ( v*v<thr ) nbelow++;
    }
    met.sparsity_ = (double)nbelow/sz;
    met.entropy_ = eng.entropy();
    met.millis_ = timer.elapsed();
    (void)level;
    return true;
}


bool GeoDevDWTCompare::addSample( const float* sig, int n )
{
    if ( !sig || n<2 ) return false;
    Sample s;
    s.data_.setSize( n );
    for ( int idx=0; idx<n; idx++ ) s.data_[idx] = sig[idx];
    samples_ += s;
    return true;
}


void GeoDevDWTCompare::addWavelet( GeoDevDWT1D::WaveletID wt )
{
    wts_ += wt;
}


void GeoDevDWTCompare::clearSamples()
{
    samples_.erase();
}


void GeoDevDWTCompare::compute()
{
    rows_.erase();
    for ( int w=0; w<wts_.size(); w++ )
    {
	Row row;
	row.wavelet_ = WaveletTransform::toString( wts_[w] );
	row.nrsamples_ = samples_.size();
	double se=0, ss=0, sh=0, st=0;
	for ( int s=0; s<samples_.size(); s++ )
	{
	    GeoDevDWT1D::Metrics met;
	    if ( !GeoDevDWT1D::evaluate(wts_[w],samples_[s].arr(),
					samples_[s].size(),level_,met) )
		continue;
	    se += met.energy_; ss += met.sparsity_;
	    sh += met.entropy_; st += met.millis_;
	}
	const int nr = samples_.size()>0 ? samples_.size() : 1;
	row.energy_ = se/nr; row.sparsity_ = ss/nr;
	row.entropy_ = sh/nr; row.millis_ = st/nr;
	rows_ += row;
    }
}


bool GeoDevDWTCompare::writeReport( od_ostream& strm ) const
{
    if ( !strm.isOK() ) return false;
    strm << "# GeoDevKit wavelet basis comparison report\n";
    strm << "# decomposition level: " << level_ << "\n";
    strm << "# trace samples: " << samples_.size() << "\n";
    strm << "# basis\tavg_energy\tavg_sparsity\tavg_entropy\tavg_millis\n";
    for ( int idx=0; idx<rows_.size(); idx++ )
    {
	const Row& r = rows_[idx];
	strm << r.wavelet_.buf() << '\t'
	     << r.energy_ << '\t' << r.sparsity_ << '\t'
	     << r.entropy_ << '\t' << r.millis_ << '\n';
    }
    strm.flush();
    return strm.isOK();
}
