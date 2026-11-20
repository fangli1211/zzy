#ifndef geodevkitdxf_h
#define geodevkitdxf_h

/*+
________________________________________________________________________

 ASCII DXF R12 写出引擎（P2 · ioDXF 模块，自包含）

 设计要点：
   1. 只依赖 Basic 层的 BufferString / od_ostream，不碰任何 EM / Geometry
      内部对象；上层对话框负责把 OpendTect 对象转换成 X/Y/Z 点列后再喂给
      本类。这样即使内核几何 API 变化，DXF 写出逻辑也不受影响。
   2. 采用 ASCII DXF R12（$ACADVER = AC1009）——兼容性最好，AutoCAD、
      LibreCAD、QCAD、国产 CAD 都能打开；实体集只实现最常用的
      LINE / POLYLINE(+VERTEX+SEQEND) / TEXT / POINT。
   3. 图层在 addXXX() 中按需登记（名称 + ACI 颜色），write() 时统一输出
      LAYER 表；坐标单位约定为米（$INSUNITS=6，$MEASUREMENT=1），
      Z 直接写层位/断层的时间或深度值，具体含义由调用方在图名中说明。
   4. MSVC 下本模块通过目标级 /utf-8 编译，中文注释与字符串安全。
   5. 禁止 strcmp / sqrt / atan2；数字格式化用 toString，避免浮点精度问题。
________________________________________________________________________
-*/

#include "bufstring.h"
#include "bufstringset.h"
#include "coord.h"
#include "typeset.h"


class od_ostream;


class GeoDevDXFWriter
{
public:
			GeoDevDXFWriter();
			~GeoDevDXFWriter();

    // 登记图层；重复登记同一名称只更新颜色。图层 0 始终存在。
    void		addLayer(const char* nm,int acicolor=7);

    // 基本实体。所有坐标一律使用 DXF 世界坐标（X/Y 为测量坐标，Z 为层位值）。
    void		addLine(const Coord3&,const Coord3&,const char* layer);
    void		addPolyline(const TypeSet<Coord3>&,bool closed,
				    const char* layer);
    void		addPoint(const Coord3&,const char* layer);
    void		addText(const Coord3&,const char* txt,double height,
				    const char* layer);

    // 交换/清空。clear() 保留图层定义，只清实体。
    void		clear();
    int			nrEntities() const	{ return nrentities_; }

    // 写出完整 DXF 文件；失败时把原因写入 errmsg。
    bool		write(const char* fnm,BufferString& errmsg) const;

private:

    void		ensureLayer(const char*);

    BufferStringSet	layers_;
    TypeSet<int>	layercolors_;
    BufferString	entitytxt_;	// 累积的 ENTITIES 段文本
    int			nrentities_ = 0;
};


#endif
