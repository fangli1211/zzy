#ifndef geodevkitdwt_h
#define geodevkitdwt_h

/*+
________________________________________________________________________

 Copyright: (C) 2026 GeoDevKit (custom plugin for OpendTect)
 License:   GPLv3+ (same as OpendTect core)
________________________________________________________________________

-*/

#include "bufstring.h"
#include "coord.h"
#include "typeset.h"
#include "wavelettrans.h"

class od_ostream;

/*!
\brief Self-contained discrete-wavelet decomposition engine for 1D signals.

Wraps the kernel ::DWT (23 wavelet bases from WaveletTransform) for the
P1 preprocessing chain. The kernel DWT performs a *full* dyadic decomposition
down to a single coefficient; this engine reuses that coefficient array and
reconstructs any requested MRA component level by masking the coefficients
of the other subbands and running the inverse transform. The result is a
component sampled on the ORIGINAL grid (same length as the input trace),
which is what "MRA component volume on the same grid" requires.

Signal length: the kernel DWT requires a power-of-two length. Input signals
are centred zero-padded up to the next power of two; on reconstruction the
first n input samples are copied back, so output grid is unchanged.

Boundary handling: the kernel FilterWT1D uses circular (wrap-around)
extension. There is no configurable boundary mode in the open-source kernel;
this is documented rather than faked with a UI option.

NOTE ON SCALING: the kernel FilterWT1D::init() normalises the low-pass
filter by sum(h^2) (=2 for most Daubechies/Symmlet bases), so the forward
transform is not orthonormal. A forward+inverse round trip reproduces the
signal SHAPE exactly but with a uniform amplitude gain (~3.2x for db4).
Subband isolation and relative energies are unaffected in shape; absolute
amplitude of an MRA component carries this known gain. Verified against the
kernel with a standalone numerical test (subband layout + reconstruction).

No OpendTect UI dependency: this class only uses Algo/Basic, so it can be
unit-tested and reused from a batch executor.
*/

class GeoDevDWT1D
{
public:
    typedef WaveletTransform::WaveletType WaveletID;

    static void		getWaveletNames(BufferStringSet&);
    static WaveletID	waveletFromIdx(int);
    static int		idxFromWavelet(WaveletID);
    static int		maxLevel(int signalsz);
			//!< number of dyadic levels available for this length

    explicit		GeoDevDWT1D(WaveletID =WaveletTransform::Daubechies4);
    ~GeoDevDWT1D();

    bool		decompose(const float* sig,int n);
			//!< forward full DWT; stores coefficients internally

    bool		reconstructApprox(int level,float* out,int n) const;
			//!< cA_level on original grid
    bool		reconstructDetail(int level,float* out,int n) const;
			//!< cD_level on original grid

    // Wavelet-domain attributes (per trace, computed from stored coeffs)
    double		approxEnergy(int level) const;
    double		detailEnergy(int level) const;
    double		detailEnergyTotal(int level) const;
			//!< sum of cD_1..cD_level energies
    double		relativeDetailEnergy(int level) const;
			//!< detailEnergyTotal / (approx+detail), 0..1
    double		entropy() const;
			//!< Shannon entropy of normalised squared coefficients

    const float*	coeffs() const		{ return coeffs_; }
    int			coeffSize() const	{ return coefsz_; }
    int			padSize() const		{ return padsz_; }
    bool		isReady() const		{ return ready_; }

    // Multi-wavelet comparison metrics (one pass over a signal)
    struct Metrics
    {
	double		energy_		= 0; //!< total coefficient energy
	double		sparsity_	= 0; //!< fraction of |c| below threshold
	double		entropy_	= 0;
	double		millis_		= 0;
    };
    static bool		evaluate(WaveletID,const float* sig,int n,int level,
				 Metrics&);
			//!< decompose once, fill metrics; no reconstruction

private:
    void		cleanup();
    bool		subbandRange(int level,bool approx,int& start,
				     int& len) const;
			//!< coefficient index range for cA(cD)_level

    WaveletID		wt_;
    float*		coeffs_		= nullptr;
    int			coefsz_		= 0;
    int			padsz_		= 0;
    int			orgsz_		= 0;
    int			maxlev_		= 0;
    bool		ready_		= false;
};


/*!
\brief Aggregate comparison report for multiple wavelet bases on a survey.

Collects per-wavelet metrics over a limited number of sampled traces so that
the D3 decision ("which basis is optimal") can be made from data instead of
a hard-coded metric. The report is plain text so it can be archived with the
deliverables.
*/
class GeoDevDWTCompare
{
public:
    explicit GeoDevDWTCompare( int level=3 ) : level_(level) {}
    ~GeoDevDWTCompare() {}

    void		setLevel(int l)		{ level_ = l; }
    int			level() const		{ return level_; }

    bool		addSample(const float* sig,int n);
    void		addWavelet(GeoDevDWT1D::WaveletID);
    void		clearSamples();

    void		compute();
    bool		writeReport(od_ostream&) const;

    struct Row
    {
	BufferString	wavelet_;
	double		energy_		= 0;
	double		sparsity_	= 0;
	double		entropy_	= 0;
	double		millis_		= 0;
	int		nrsamples_	= 0;

	bool operator==( const Row& o ) const
	{ return wavelet_==o.wavelet_ && energy_==o.energy_
	      && sparsity_==o.sparsity_ && entropy_==o.entropy_
	      && millis_==o.millis_ && nrsamples_==o.nrsamples_; }
	bool operator!=( const Row& o ) const	{ return !(*this==o); }
    };

    const TypeSet<Row>& rows() const	{ return rows_; }

private:
    struct Sample
    {
	TypeSet<float>	data_;
	const float*	arr() const { return data_.arr(); }
	int		size() const	{ return data_.size(); }
	bool operator==( const Sample& o ) const { return data_==o.data_; }
	bool operator!=( const Sample& o ) const { return !(*this==o); }
    };

    int			level_ = 3;
    TypeSet<Sample>	samples_;
    TypeSet<WaveletTransform::WaveletType> wts_;
    TypeSet<Row>	rows_;
};

#endif
