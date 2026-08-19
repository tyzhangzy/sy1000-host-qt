# -*- coding: utf-8 -*-
# Correct zh_CN translations to match the original WPF terminology.
import re

path = "i18n/sy1000_zh_CN.ts"

# source -> corrected 中文 (overrides existing translations for these sources)
FIX = {
    "External": "外部检查",
    "Internal": "内部检查",
    "Thread": "瓶口螺纹检查",
    "Valve": "气瓶阀检查",
    "Thermal damage": "热损伤迹象",
    "Scratch": "有无划伤",
    "Wear": "有无磨损",
    "Delamination": "有无分层",
    "Deformation": "有无异常变形",
    "Smell present": "有无气味",
    "Defect location": "缺陷位置描述",
    "Debris": "杂物的种类和数量",
    "Surface condition": "内表面状况描述",
    "Thread specification": "螺纹规格",
    "Thread condition": "螺纹状况描述",
    "Thread evaluation": "螺纹状况评估",
    "Valve No": "气瓶阀编号",
    "Valve thread condition": "连接螺纹状况描述",
    "Air tightness": "气密状况描述",
    "Diaphragm replaced": "爆破片是否更换",
    "To Repair": "待修复",
    "To Replace": "待更换",
    "Scrapped": "判废",
    "Result": "评定结果",
    "Result Management": "试验结果管理",
    "Connection Status": "连接状态",
    "Start Hydrostatic Test": "开始水压试验",
    "User Management": "用户管理",
    "System Maintain": "系统维护",
    "Log out": "退出登录",
    "Log out Menu": "登出菜单",
    "Start": "开始试验",
    "Stop": "停止试验",
    "Add": "创建新用户",
    "Volume (L)": "气瓶容积",
    "External:": "外部检查：",
    "Internal:": "内部检查：",
    "Thread:": "瓶口螺纹检查：",
    "Valve:": "气瓶阀检查：",
    # Test preparation page (WPF StandardCard / SampleCard terminology).
    "Input Test Standard": "输入试验标准",
    "Working Pressure": "公称工作压力",
    "Testing Pressure": "试验压力",
    "Hold Time": "保压时间",
    "sec": "秒",
    "Residual Deformation Rate": "允许容积残余变形率",
    "Confirm": "确 定",
    "Input Cylinder %1 Info": "输入 %1 号气瓶信息",
    "Cylinder Model": "气瓶型号",
    "Cylinder Volume": "气瓶容积",
    "User Company": "使用单位",
    "Appearance Inspection": "外观检测评估",
    "Start Test": "开始试验",
    "Back": "返回",
    "OK": "确定",
    "Please enter cylinder model.": "请输入气瓶型号。",
    "Please enter manufacturer.": "请输入制造厂商。",
    "Please enter a valid cylinder volume.": "请输入有效的气瓶容积。",
    "Please enter user company.": "请输入使用单位。",
    "Please enter serial No.": "请输入产品编号。",
    "Please complete appearance inspection before saving.": "请完成外观检测评估后再保存。",
    # Hydrostatic test page (WPF MainTestWindow terminology).
    "Cylinder pressure": "气瓶压力",
    "Status: ": "状态：",
    "%1 cylinder": "%1 号气瓶",
    "Save Test Result": "保存试验结果",
    "View Test Report": "查看试验报告",
    "Return to Main Menu": "返回主菜单",
    "Finished ok=%1, passed=%2, failed=%3": "完成 ok=%1，通过=%2，失败=%3",
    "Hydrostatic Test": "水压试验",
}


with open(path, encoding="utf-8") as f:
    content = f.read()

def repl(m):
    src = m.group(1)
    if src in FIX:
        return '<source>' + src + '</source>\n        <translation>' + FIX[src] + '</translation>'
    return m.group(0)

new = re.sub(
    r'<source>(.*?)</source>\s*\n\s*<translation[^>]*>.*?</translation>',
    repl, content)

with open(path, "w", encoding="utf-8") as f:
    f.write(new)
print("corrected done")

