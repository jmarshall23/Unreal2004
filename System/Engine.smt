[Public]
;Object=(Name=Engine.Console,Class=Class,MetaClass=Engine.Console)
Object=(Name=Engine.ServerCommandlet,Class=Class,MetaClass=Core.Commandlet)
Object=(Name=Engine.MasterMD5Commandlet,Class=Class,MetaClass=Core.Commandlet)
Preferences=(Caption="进阶",Parent="进阶设定")
Preferences=(Caption="游戏引擎设定",Parent="进阶",Class=Engine.GameEngine,Category=Settings,Immediate=True)
Preferences=(Caption="Key Aliases",Parent="进阶",Class=Engine.Input,Immediate=True,Category=Aliases)
Preferences=(Caption="Raw Key Bindings",Parent="进阶",Class=Engine.Input,Immediate=True,Category=RawKeys)
Preferences=(Caption="驱动程序",Parent="进阶设定",Class=Engine.Engine,Immediate=False,Category=Drivers)
Preferences=(Caption="公开服务器资料",Parent="网络",Class=Engine.GameReplicationInfo,Immediate=True)
Preferences=(Caption="游戏设定",Parent="进阶设定",Class=Engine.GameInfo,Immediate=True)

[Errors]
NetOpen="开启档案时发生错误"
NetWrite="写入档案时发生错误"
NetRefused="服务器拒绝传送 '%s'"
NetClose="关闭档案时发生错误"
NetSize="档案大小不合"
NetMove="移动档案时发生错误"
NetInvalid="接收到无效的档案要求"
NoDownload="Package '%s' 无法下载"
DownloadFailed="下载 package '%s' 失败: %s"
RequestDenied="服务器要求下一关卡档案: 拒绝"
ConnectionFailed="连接失败"
ChAllocate="无法配置频道"
NetAlready="已经连结"
NetListen="接听失败: 没有连结内容"
LoadEntry=""无法加载入口: %s"
InvalidUrl="无效的 URL: %s"
InvalidLink="无效的连结: %s"
FailedBrowse="进入 %s 失败: %s"
Listen="接听失败: %s"
AbortToEntry="失败; 返回入口"
ServerOpen="服务器无法打开网络 URL"
ServerListen="专用服务器无法接听: %s"
Pending="连接 '%s' 的等待已经失败; %s"
LoadPlayerClass="玩者 class 加载失败"
ServerOutdated="服务器的版本已经过时"
ClientOutdated="你需要更新到最新的版本"
InvalidCDKey="你的 CD 序号是无效的.  你需要重新安装游戏并输入 CD 序号才能解决这个问题."
ConnectLost="失去联机"

[UpgradeDrivers]
OutdatedDrivers="你计算机中的显示卡驱动程序为旧版且可能无法对应这个游戏."
OursOrWeb="选择 '是' 从 CD 上更新或 '否' 来连到制造商网站"
InsertCD="请放入你的浴血战场 CD 3"
NvidiaURL="http://www.nvidia.com/content/drivers/drivers.asp"
AtiURL="http://www.ati.com/support/driver.html"
CDButton="从 CD 更新"
WebButton="从网络更新"
cancelButton="不要更新"

[KeyNames]
Up="上"
Right="右"
Left="左"
Down="下"
LeftMouse="鼠标左键"
RightMouse="鼠标右键"
MiddleMouse="鼠标中键"
MouseX="鼠标X轴"
MouseY="鼠标Y轴"
MouseZ="鼠标Z轴"
MouseW="鼠标W轴"
JoyX="摇杆X轴"
JoyY="摇杆Y轴"
JoyZ="摇杆Z轴"
JoyU="摇杆U轴"
JoyV="摇杆V轴"
JoySlider1="摇杆滑杆1"
JoySlider2="摇杆滑杆2"
MouseWheelUp="鼠标滚轮上"
MouseWheelDown="鼠标滚轮下"
Joy1="摇杆1钮"
Joy2="摇杆2钮"
Joy3="摇杆3钮"
Joy4="摇杆4钮"
Joy5="摇杆5钮"
Joy6="摇杆6钮"
Joy7="摇杆7钮"
Joy8="摇杆8钮"
Joy9="摇杆9钮"
Joy10="摇杆10钮"
Joy11="摇杆11钮"
Joy12="摇杆12钮"
Joy13="摇杆13钮"
Joy14="摇杆14钮"
Joy15="摇杆15钮"
Joy16="摇杆16钮"
Space="空格键"
PageUp="PAGE UP"
PageDown="PAGE DOWN"
End="END"
Home="HOME"
Select="SELECT"
Print="PRINT"
Execute="执行"
PrintScrn="PRINTSCRN"
Insert="INSERT"
Delete="DELETE"
Help="说明"
NumPad0="NUM 0"
NumPad1="NUM 1"
NumPad2="NUM 2"
NumPad3="NUM 3"
NumPad4="NUM 4"
NumPad5="NUM 5"
NumPad6="NUM 6"
NumPad7="NUM 7"
NumPad8="NUM 8"
NumPad9="NUM 9"
GreyStar="GREY STAR"
GreyPlus="GREY PLUS"
Separator="SEPARATOR"
GreyMinus="GREY MINUS"
NumPadPeriod="NUM ."
GreySlash="GREY SLASH"
Pause="PAUSE"
CapsLock="CAPSLOCK"
Tab="TAB"
Enter="ENTER"
Shift="SHIFT"
NumLock="NUMLOCK"
Escape="ESCAPE"
JoySlider2="JOYSLIDER 2"

[Progress]
CancelledConnect="取消的连结尝试"
RunningNet="%s: %s (%i 玩者)"
NetReceiving="接收 '%s': %i/%i"
NetReceiveOk="成功接收 '%s'"
NetSend="传送 '%s'"
NetSending="传送 '%s': %i/%i"
Connecting="连接中..."
Listening="接听玩者中..."
Loading="加载中"
Saving="储存中"
Paused="%s 暂停"
ReceiveFile="接收 '%s' (F10 取消)"
ReceiveOptionalFile="接收非必须档案 '%s' (按 F10 跳过)"
ReceiveSize="大小 %iK, 完成 %3.1f%%"
ConnectingText=""
ConnectingURL="unreal://%s/%s"
CorruptConnect="发现联机损坏"

[ServerCommandlet]
HelpCmd=server
HelpOneLiner="网络游戏服务器"
HelpUsage=server map.unr[?game=gametype] [-option...] [parm=value]...
HelpWebLink=http://unreal.epicgames.com/servertips.htm
HelpParm[0]=Log
HelpDesc[0]="指定产生的日志纪录文件"
HelpParm[1]=AllAdmin
HelpDesc[1]="给所有的玩者管理权限"

[MasterMD5Commandlet]
HelpCmd=mastermd5 [*.ext {*.ext ...} ]
HelpOneLiner="产生主要 MD5 表格"
HelpUsage=mastermd5
HelpWebLink=http://unreal.epicgames.com/servertips.htm

[General]
Upgrade="你必须先更新到最新版本才能进入这个服务器, Epic 网站提供浴血战场免费下载服务:"
UpgradeURL="http://www.unreal.com/upgrade"
UpgradeQuestion="你要开启你的浏览器并前往更新下载的网页?"
Version="版本 %i"

[AccessControl]
IPBanned="你的 IP 被服务器禁止了."
WrongPassword="你输入的密码不正确."
NeedPassword="你必须输入密码才能进入游戏."
SessionBanned="你的 IP 被目前进行的游戏禁止了."
KickedMsg="你被强制离开游戏."

[Ammo]
PickupMessage="你捡起一些弹药."

[Crushed]
DeathString="%o 被 %k 压扁了."
FemaleSuicide="%o 被压扁了."
MaleSuicide="%o 被压扁了."

[DamTypeTelefragged]
DeathString="%o 被 %k 给传送撕裂了"
FemaleSuicide="%o 被 %k 给传送撕裂了"
MaleSuicide="%o 被 %k 给传送撕裂了"

[DamageType]
DeathString="%o 被 %k 杀了."
FemaleSuicide="%o 杀了她自己."
MaleSuicide="%o 杀了他自己."

[FailedConnect]
FailMessage[0]="加入游戏失败.  需要密码."
FailMessage[1]="加入游戏失败.  密码错误."
FailMessage[2]="加入游戏失败.  游戏已经开始."
FailMessage[3]="加入游戏失败."

[FellLava]
DeathString="%o 坠落并烧焦了"
FemaleSuicide="%o 坠落并烧焦了"
MaleSuicide="%o 坠落并烧焦了"

[GameInfo]
bAlternateMode=False
DefaultPlayerName="玩者"
GameName="游戏"
GIPropsDisplayText[0]="计算机人技巧"
GIPropsDisplayText[1]="留下武器"
GIPropsDisplayText[2]="使用地图轮替"
GIPropsDisplayText[3]="血腥程度"
GIPropsDisplayText[4]="游戏速度"
GIPropsDisplayText[5]="最大旁观者数目"
GIPropsDisplayText[6]="最大玩者数目"
GIPropsDisplayText[7]="得分分数"
GIPropsDisplayText[8]="最大生命数目"
GIPropsDisplayText[9]="时间限制"
GIPropsDisplayText[10]="世界统计日志纪录"
GIPropsExtras[0]="0;新手;1;一般;2;进阶;3;技巧;4;熟练;5;专家;6;超人;7;神"
GIPropsExtras[1]="0;充满血腥;1;少量血腥"

[GameMessage]
SwitchLevelMessage="更换关卡"
LeftMessage=" 离开游戏."
FailedTeamMessage="无法找到队伍给玩者"
FailedPlaceMessage="无法找到起点"
FailedSpawnMessage="无法再生"
EnteredMessage=" 进入游戏."
MaxedOutMessage="服务器已经满载."
OvertimeMessage="正规赛程平手. 猝死延长赛!!!"
GlobalNameChange="改名为"
NewTeamMessage="正在"
NoNameChange="名称已经被使用."
VoteStarted="提议进行投票."
VotePassed="投票通过."
MustHaveStats="必须开启统计才能加入此服务器."
NewPlayerMessage="一位新玩者加入了游戏."

[HUD]
ProgressFontName="UT2003Fonts_smt.SimpMandFont12"
OverrideConsoleFontName="UT2003Fonts_smt.SimpMandFontIRC"

[GameProfile]
PositionName[0]="自动指定"
PositionName[1]="防御"
PositionName[2]="进攻"
PositionName[3]="自由"
PositionName[4]="支持"

[GameReplicationInfo]
GRIPropsDisplayText[0]="服务器名称"
GRIPropsDisplayText[1]="服务器短名"
GRIPropsDisplayText[2]="管理员名称"
GRIPropsDisplayText[3]="管理员电子邮件"
GRIPropsDisplayText[4]="讯息"
GRIPropsDisplayText[5]=" - "
GRIPropsDisplayText[6]="本"
GRIPropsDisplayText[7]="日"

[Gibbed]
DeathString="%o 炸的血肉横飞"
FemaleSuicide="%o 炸的血肉横飞"
MaleSuicide="%o 炸的血肉横飞"

[LevelInfo]
Title="无标题"

[MatSubAction]
Desc="无"

[Mutator]
FriendlyName="突变模块"
Description="叙述"

[Pickup]
PickupMessage="拿到一个物品."

[PlayerController]
QuickSaveString="快速储存"
NoPauseMessage="游戏无法暂停"
ViewingFrom="目前观看视野为"
OwnCamera="从摄影机视野观看"

[PlayerReplicationInfo]
StringSpectating="观察中"
StringUnknown="不明"

[SubActionCameraEffect]
Desc="摄影机效果"

[SubActionCameraShake]
Desc="摇动"

[SubActionFOV]
Desc="FOV"

[SubActionFade]
Desc="淡化"

[SubActionGameSpeed]
Desc="游戏速度"

[SubActionOrientation]
Desc="方向"

[SubActionSceneSpeed]
Desc="场景速度"

[SubActionTrigger]
Desc="触发"

[Suicided]
DeathString="%o 长了动脉瘤."
FemaleSuicide="%o 长了动脉瘤."
MaleSuicide="%o 长了动脉瘤."

[TeamInfo]
TeamName="队伍"
ColorNames[0]="红"
ColorNames[1]="蓝"
ColorNames[2]="绿"
ColorNames[3]="黄"

[Volume]
LocationName="未定义"

[Weapon]
MessageNoAmmo=" 没有弹药"

[WeaponPickup]
PickupMessage="你拿到一个武器"

[XBoxPlayerInput]
LookPresets[0].PresetName="线型"
LookPresets[1].PresetName="倍速"
LookPresets[2].PresetName="混合"
LookPresets[3].PresetName="自订"

[fell]
DeathString="%o 撞出了一个凹洞"
FemaleSuicide="%o 撞出了一个凹洞"
MaleSuicide="%o 撞出了一个凹洞"

