实现摄像头

读取（camkit调用v4l2）
使用ffmpeg转码（yuv422—yuv420）
预览（opengl渲染yuv通过qml_opengl）
存储(通过camkit调用硬件编码)


解决了gpu内存泄露的问题（将gentexture放在初始化里）