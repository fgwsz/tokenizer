这是一个简单的命令行工具,它是一个分词器.
    use: <input file path>
    use: <input file path>...
    use: <input file path> -o <output file path>
    use: <input file path>... -o <output file path>;

这个工具的功能:
    输入一个或多个文件路径,就可以得到其中出现的英文单词的频率排行榜.
    可以使用可选项[-o] {output file path}来输出到指定文件,
    当不使用[-o]时默认输出到控制台.


工具测试:
    Linux系统:
        ./build.sh
        ./tokenizer ./chap01.md
        ./tokenizer ./chap01.md ./README.md
        ./tokenizer ./chap01.md -o ./output.txt
        ./tokenizer ./chap01.md ./README.md -o ./output.txt
    Windows系统:
        ./build.ps1
        ./tokenizer.exe ./chap01.md
        ./tokenizer.exe ./chap01.md ./README.md
        ./tokenizer.exe ./chap01.md -o ./output.txt
        ./tokenizer.exe ./chap01.md ./README.md -o ./output.txt
