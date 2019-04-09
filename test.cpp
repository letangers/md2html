#include <fstream>         // std::ofstream
#include "md2html.h"  // 需要实现的 Markdown 解析类

int main() {
    // 装载构造 Markdown 文件
	md2html parser("test.md");
	parser.ToHtml();
    
    
    // 准备要写入的 HTML 文件头尾信息
    std::string head = "<!DOCTYPE html><html><head>\
        <meta charset=\"utf-8\">\
        <title>Markdown</title>\
        <link rel=\"stylesheet\" href=\"github-markdown.css\">\
        </head><body><article class=\"markdown-body\">";
    std::string end = "</article></body></html>";
    
    // 将结果写入到文件
    std::ofstream out;
    out.open("output/index.html");
    // 将转换后的内容构造到 HTML 的 <article></article> 标签内部
    out << head << parser.content <<end;
    out.close();
    return 0;
}
