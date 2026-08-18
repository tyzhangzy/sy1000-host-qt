# report/

对应原 WPF 的 `TestReportGenerator.cs`。报告生成。

计划迁移内容：
- 曲线图截图（原 `SKCartesianChart` 导出）→ QML `QQuickPaintedItem` 曲线导出为图片
- PDF 报告排版（原 PDFsharp 手写排版）→ `QTextDocument::print` / `QPrinter`
- 测试报告查看（原 WebView2 显示 PDF）→ QML 内嵌 PDF / `QPdfView`
