/*+
________________________________________________________________________

 ASCII DXF R12 写出引擎实现

 文件格式要点（对照 Autodesk DXF R12 规范）：
   - 全部为「组码 + 值」两行一组；组码整数独占一行，值紧随其后。
   - SECTION 依次写 HEADER / TABLES / ENTITIES，最后 EOF。
   - HEADER 里声明 AC1009（R12）、INSUNITS=6（米）、MEASUREMENT=1（公制）。
   - TABLES 只写 LTYPE(CONTINUOUS) 与 LAYER，足够 CAD 正确分色显示。
   - POLYLINE 用经典的 POLYLINE + 若干 VERTEX + SEQEND 三段式表达，
     R12 不支持 LWPOLYLINE，务必不要混用。

 数字格式化：BufferString::add(double) 输出「干净数字」（去掉多余 0），
 对 X/Y 这类可能很大的测量坐标也能完整保留有效位；这里无需 addPrecise。
________________________________________________________________________
-*/

#include "geodevkitdxf.h"

#include "bufstringset.h"
#include "od_ostream.h"
#include "string2.h"


GeoDevDXFWriter::GeoDevDXFWriter()
{
    addLayer( "0", 7 ); // DXF 规范保留图层
}


GeoDevDXFWriter::~GeoDevDXFWriter()
{}


void GeoDevDXFWriter::addLayer( const char* nm, int acicolor )
{
    if ( !nm || !*nm )
	nm = "0";
    const int idx = layers_.indexOf( nm );
    if ( idx>=0 )
	layercolors_[idx] = acicolor;
    else
    {
	layers_.add( nm );
	layercolors_.add( acicolor );
    }
}


void GeoDevDXFWriter::ensureLayer( const char* nm )
{
    if ( layers_.indexOf( nm )<0 )
	addLayer( nm );
}


void GeoDevDXFWriter::clear()
{
    entitytxt_.setEmpty();
    nrentities_ = 0;
}


// 追加一对「组码 / 值」。所有数值先转成 BufferString，避免 locale 影响。
static void addPair( BufferString& bs, int code, const char* val )
{
    bs.add( code ).add( "\n" ).add( val ).add( "\n" );
}


static void addPairI( BufferString& bs, int code, int val )
{
    BufferString tmp; tmp.add( val );
    addPair( bs, code, tmp.buf() );
}


static void addPairD( BufferString& bs, int code, double val )
{
    // 'g' 格式以最少字符保留有效数字；对大地坐标与毫秒时间都适用。
    addPair( bs, code, toString( val, 0, 'g', 10 ) );
}


void GeoDevDXFWriter::addLine( const Coord3& a, const Coord3& b,
			       const char* layer )
{
    ensureLayer( layer );
    entitytxt_.add( "0\nLINE\n8\n" ).add( layer ).add( "\n" );
    addPairD( entitytxt_, 10, a.x_ );
    addPairD( entitytxt_, 20, a.y_ );
    addPairD( entitytxt_, 30, a.z_ );
    addPairD( entitytxt_, 11, b.x_ );
    addPairD( entitytxt_, 21, b.y_ );
    addPairD( entitytxt_, 31, b.z_ );
    nrentities_++;
}


void GeoDevDXFWriter::addPoint( const Coord3& p, const char* layer )
{
    ensureLayer( layer );
    entitytxt_.add( "0\nPOINT\n8\n" ).add( layer ).add( "\n" );
    addPairD( entitytxt_, 10, p.x_ );
    addPairD( entitytxt_, 20, p.y_ );
    addPairD( entitytxt_, 30, p.z_ );
    nrentities_++;
}


void GeoDevDXFWriter::addPolyline( const TypeSet<Coord3>& pts, bool closed,
				   const char* layer )
{
    if ( pts.size()<2 )
	return;
    ensureLayer( layer );

    entitytxt_.add( "0\nPOLYLINE\n8\n" ).add( layer ).add( "\n" );
    addPairI( entitytxt_, 66, 1 ); // 后面跟随顶点
    addPairI( entitytxt_, 70, closed ? 1 : 0 );
    addPairD( entitytxt_, 10, 0. );
    addPairD( entitytxt_, 20, 0. );
    addPairD( entitytxt_, 30, 0. );

    for ( int idx=0; idx<pts.size(); idx++ )
    {
	const Coord3& p = pts[idx];
	entitytxt_.add( "0\nVERTEX\n8\n" ).add( layer ).add( "\n" );
	addPairD( entitytxt_, 10, p.x_ );
	addPairD( entitytxt_, 20, p.y_ );
	addPairD( entitytxt_, 30, p.z_ );
    }

    entitytxt_.add( "0\nSEQEND\n8\n" ).add( layer ).add( "\n" );
    nrentities_++;
}


void GeoDevDXFWriter::addText( const Coord3& p, const char* txt,
			       double height, const char* layer )
{
    if ( !txt || !*txt )
	return;
    ensureLayer( layer );
    entitytxt_.add( "0\nTEXT\n8\n" ).add( layer ).add( "\n" );
    addPairD( entitytxt_, 10, p.x_ );
    addPairD( entitytxt_, 20, p.y_ );
    addPairD( entitytxt_, 30, p.z_ );
    addPairD( entitytxt_, 40, height );
    addPair( entitytxt_, 1, txt );
    nrentities_++;
}


bool GeoDevDXFWriter::write( const char* fnm, BufferString& errmsg ) const
{
    od_ostream strm( fnm );
    if ( !strm.isOK() )
    {
	errmsg = "Cannot open DXF file for writing";
	return false;
    }

    BufferString out;
    out.add( "0\nSECTION\n2\nHEADER\n" );
    addPair( out, 9, "$ACADVER" );
    addPair( out, 1, "AC1009" );
    addPair( out, 9, "$INSUNITS" );
    addPairI( out, 70, 6 );   // 米
    addPair( out, 9, "$MEASUREMENT" );
    addPairI( out, 70, 1 );   // 公制
    addPair( out, 9, "$LUNITS" );
    addPairI( out, 70, 2 );   // 十进制
    out.add( "0\nENDSEC\n" );

    // ---- TABLES ----
    out.add( "0\nSECTION\n2\nTABLES\n" );
    out.add( "0\nTABLE\n2\nLTYPE\n70\n1\n" );
    out.add( "0\nLTYPE\n2\nCONTINUOUS\n70\n0\n3\nSolid line\n"
	     "72\n65\n73\n0\n40\n0.0\n" );
    out.add( "0\nENDTAB\n" );

    out.add( "0\nTABLE\n2\nLAYER\n" );
    addPairI( out, 70, sCast(int,layers_.size()) );
    for ( int idx=0; idx<layers_.size(); idx++ )
    {
	out.add( "0\nLAYER\n2\n" ).add( layers_.get(idx) ).add( "\n" );
	addPairI( out, 70, 0 );
	addPairI( out, 62, layercolors_[idx] );
	addPair( out, 6, "CONTINUOUS" );
    }
    out.add( "0\nENDTAB\n" );
    out.add( "0\nENDSEC\n" );

    // ---- ENTITIES ----
    out.add( "0\nSECTION\n2\nENTITIES\n" );
    out.add( entitytxt_ );
    out.add( "0\nENDSEC\n0\nEOF\n" );

    strm << out.buf();
    strm.close();
    if ( !strm.isOK() )
    {
	errmsg = "Failed to write DXF content";
	return false;
    }
    return true;
}
