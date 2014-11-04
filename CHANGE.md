修改说明
=========

* 修改LOG_LEVEL_系列宏定义，改为DUIV_LOG_LEVEL_，免和其他的LOG库定义冲突
* 只维护VS2010的解决方案
* 复选/单选菜单项：点击后菜单关闭
* 延迟响应点击托盘图标，以区分“双击”和“单击”,可以通过cfg中trayLbClickMsgDelay启用
* 增加对弹出菜单设置Handler，这样可以通过Handler设置弹出菜单的初始状态