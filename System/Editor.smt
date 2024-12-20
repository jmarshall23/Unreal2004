[Public]
Object=(Name=Editor.MasterCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.MakeCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.ConformCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.BatchExportCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.MergeDXTCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.PackageFlagCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.DataRipCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.PkgCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.MapConvertCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.DXTConvertCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.AnalyzeContentCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=PSX2Convert.PSX2ConvertCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=PSX2Convert.PSX2MusicCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=GCNConvert.GCNConvertCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.AnalyzeBuildCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.ConvertMaterialCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.TextureLODCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.RebuildCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.DumpIntCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Editor.SetNormalLODCommandlet,Class=Class,MetaClass=Core.Commandlet)
Preferences=(Caption="编辑器",Parent="进阶设定")
Preferences=(Caption="进阶",Parent="编辑器",Class=Editor.EditorEngine,Immediate=True,Category=Advanced)
Preferences=(Caption="色彩",Parent="编辑器",Class=Editor.EditorEngine,Immediate=True,Category=Colors)
Preferences=(Caption="网格线",Parent="编辑器",Class=Editor.EditorEngine,Immediate=True,Category=Grid)
Preferences=(Caption="旋转网格线",Parent="编辑器",Class=Editor.EditorEngine,Immediate=True,Category=RotationGrid)

[MasterCommandlet]
HelpCmd=master
HelpOneLiner="建立主要安装档案"
HelpUsage=master [-option...] [parm=value]...
HelpParm[0]=MasterPath
HelpDesc[0]="来源档案的根目录"
HelpParm[1]=SrcPath
HelpDesc[1]="复制档案的目标根目录"
HelpParm[2]=RefPath
HelpDesc[2]="delta-压缩路径关联"

[MapConvertCommandlet]
HelpCmd=mapconvert
HelpOneLiner="转换旧格式地图到新格式 (无结构型变)"
HelpUsage=mapconvert SrcFilename DstFilename
HelpParm[0]=SrcFilename
HelpDesc[0]="地图读取来源档名"
HelpParm[1]=DstFilename
HelpDesc[1]="地图写入目标文件名"

[ConformCommandlet]
HelpCmd=conform
HelpOneLiner="产生转换二进制档案"
HelpUsage=conform existing_file.ext old_file.ext
HelpParm[0]=existingfile.ext
HelpDesc[0]="已存二进制文件加载, 转换, 及储存"
HelpParm[1]=oldfile.ext
HelpDesc[1]="使来源文件二进制兼容的旧档案"

[PkgCommandlet]
HelpCmd=pkg
HelpOneLiner="使用目录架构汇入/汇出资料从/到 packages."
HelpUsage=pkg [import/export] [texture/sound] [packagename] [directory]
HelpParm[0]=import/export
HelpDesc[0]="你想尝试对资料作什么"
HelpParm[1]=texture/sound
HelpDesc[1]="你工作中的 package 类型"
HelpParm[2]=packagename
HelpDesc[2]="你创造/汇出的 package"
HelpParm[3]=directory
HelpDesc[3]="读取/写入的目录"

[PackageFlagCommandlet]
HelpCmd=packageflag
HelpOneLiner="设定 package 标记于 package 档案"
HelpUsage=packageflag src.ext [dest.ext <+|->flag [<+|->flag] ...]
HelpParm[0]=src.ext
HelpDesc[0]="已存在的 package 档案加载"
HelpParm[1]=dest.exe
HelpDesc[1]="新 package 档案的文件名以及标记设定"
HelpParm[2]=flag
HelpDesc[2]="+ 键设定标记, 或 - 键移除标记, 之后加上以下之一:"
HelpParm[3]=" "
HelpDesc[3]="  允许下载"
HelpParm[4]=" "
HelpDesc[4]="  用户选用"
HelpParm[5]=" "
HelpDesc[5]="  服务器端使用"
HelpParm[6]=" "
HelpDesc[6]="  中断连结"
HelpParm[7]=" "
HelpDesc[7]="  非保全的"

[MakeCommandlet]
HelpCmd=make
HelpOneLiner="重建 UnrealScript packages"
HelpUsage=make [-option...] [parm=value]...
HelpParm[0]=Silent
HelpDesc[0]="无询问; 所有问题都回答 '是'"
HelpParm[1]=NoBind
HelpDesc[1]="不强迫本地函式连结 DLLs"
HelpParm[2]=All
HelpDesc[2]="完整重建 "

[DXTConvertCommandlet]
HelpCmd=dxtconvert
HelpOneLiner="转换 DXT3/5 材质格式到 DXT1/ RGBA"
HelpUsage=dxtconvert srcpath destpath
HelpParm[0]=srcpath
HelpDesc[0]="含有来源文件的路径"
HelpParm[1]=destpath
HelpDesc[1]="目标文件的路径"


[AnalyzeContentCommandlet]
HelpCmd=analyzecontent
HelpOneLiner="分析地图"
HelpUsage=analyzecontent mapname
HelpParm[0]=mapname
HelpDesc[0]="地图文件路径"


[BatchExportCommandlet]
HelpCmd=batchexport
HelpOneLiner="纯汇出对象"
HelpUsage=batchexport package.ext classname export_ext
HelpParm[0]=package.ext
HelpDesc[0]="选择 Package 对象汇出"
HelpParm[1]=classname
HelpDesc[1]="对象 Class 汇出"
HelpParm[2]=export_ext
HelpDesc[2]="汇出档案扩展名"
HelpParm[3]=path
HelpDesc[3]="汇出档案路径, 例如 c:\MyPath"

[DataRipCommandlet]
HelpCmd=datarip
HelpOneLiner="创造不含材质, 音乐, 音效的的 package."
HelpUsage=datarip srcpackage.ext dstpackage.ext
HelpParm[0]=srcpackage.ext
HelpDesc[0]="来源 Package"
HelpParm[1]=dstpackage.ext
HelpDesc[1]="目标 Package"

