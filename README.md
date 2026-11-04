# 上班攒钱时钟 · MoneyClock

> 每一秒都在变富，深圳打工人加油。

一个帮你看着工资一点点涨起来的桌面小工具。上班期间实时显示「今日已赚」，下班前有倒计时，还帮你算清楚**扣掉五险一金和个税之后到底到手多少**。

![Windows 桌面应用](https://img.shields.io/badge/platform-Windows-0078D4?style=flat-square)
![零依赖](https://img.shields.io/badge/dependency-none-success?style=flat-square)
![离线可用](https://img.shields.io/badge/network-offline%20available-lightgrey?style=flat-square)

---

## ✨ 功能

### 核心
- **今日已赚实时增长** — 每秒跳动，精确到小数点后 4 位
- **税前 / 税后双显示** — 同时显示两个赚钱速度，到手多少一目了然
- **下班倒计时** — 上班中显示距离下班，午休时自动切换为休息结束倒计时
- **本月累计** — 按当月排班工时自动折算，已过去的每天都会累加

### 针对深圳打工人的适配
- **五险一金估算** — 按深圳标准：养老 8% + 医疗 2% + 失业 0.5% = 10.5%，并考虑社保缴费基数上限
- **个税累进计算** — 起征点 5000 元/月，七级累进税率
- **预计到手金额** — 税前 → 五险一金 → 个税 → 到手，明细逐项列出
- **发工资倒计时** — 距离下次发薪日还有几天（默认 15 号，可自定义）
- **薪资快捷按钮** — 8K / 12K / 15K / 20K / 25K / 30K+ 一键切换

### 排班制度
支持全部常见制度：

| 制度 | 说明 |
|---|---|
| 双休 | 周六、周日休息 |
| 单休 | 每周指定一天休息 |
| 大小周 | 单双休交替，可选锚定日期 |
| 5.5 天 | 周六上午上班，周日下午下班 |
| 自定义 | 手动勾选每周上班日 |

其他：上下班时间可跨天（如 22:00 — 06:00），可扣除午休时段，金额跳动方式可设为「每赚 1 分 / 1 毛 / 1 块跳动一次」或实时细算。

---

## 🚀 使用方式

### 方式一：直接运行 exe（推荐）

下载 `MoneyClock.exe` 双击即可。程序会把页面写到 `%LOCALAPPDATA%\MoneyMoneyClock\index.html`，然后用 Edge 的应用模式打开——看起来就是个独立桌面程序，没有浏览器地址栏。

### 方式二：浏览器打开

直接用浏览器打开 `index.html`，功能完全一样（设置同样保存在浏览器 localStorage）。

---

## 🔧 从源码构建

项目只有一个 C 文件和一个 HTML 文件，编译命令：

```bash
# 需要 MinGW-w64
gcc -O2 -municode -o MoneyClock.exe moneyclock.c -lshell32
```

`moneyclock.c` 会把 `index.html` 以 C 字符串形式嵌入 exe，运行时释放到本地再交给浏览器渲染。

---

## 📁 项目结构

```
money-clock/
├── index.html        # 应用本体（单文件，零依赖）
├── moneyclock.c      # 桌面启动器源码
├── build.bat         # Windows 一键编译脚本
├── Makefile          # GNU Make 构建脚本
├── LICENSE           # MIT 许可证
└── README.md
```

---

## ❓ 常见问题

**Q：数据存在哪里？**
A：设置保存在浏览器的 localStorage（键名 `money-clock-v2`），不上传任何服务器，纯本地运行。

**Q：算得准吗？**
A：这是**娱乐性质的估算工具**，不计入法定节假日、调休、请假、加班费、专项附加扣除等，实际以工资条为准。

**Q：需要联网吗？**
A：不需要。首次打开后完全离线可用。

**Q：一定要用 Edge 吗？**
A：exe 默认查找 Edge（Chromium 内核）。没装的话可以用浏览器直接打开 `index.html`。

---

## 📄 License

MIT License — 详见 [LICENSE](LICENSE)。

> ⚠️ 本项目仅供学习交流与个人娱乐使用，不构成任何税务或财务建议。
