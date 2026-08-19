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

