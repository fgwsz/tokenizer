#include<cstddef>       // ::std::size_t
#include<algorithm>     // ::std::greater
#include<string>        // ::std::string
#include<unordered_map> // ::std::unoredered_map
#include<map>           // ::std::multimap
#include<iostream>      // ::std::cout
#include<fstream>       // ::std::ifstream
#include<sstream>       // ::std::stringstream
#include<stdexcept>     // ::std::runtime_error

// 全局变量
static ::std::unordered_map<::std::string,::std::size_t> data_base={};
static ::std::multimap<
    ::std::size_t
    ,::std::string
    ,::std::greater<::std::size_t>
> output={};
// 检测函数
bool is_letter(char const ch){
    auto uc=static_cast<unsigned char>(ch);
    return (uc>='A'&&uc<='Z')||(uc>='a'&&uc<='z');
}
bool is_number(char const ch){
    auto uc=static_cast<unsigned char>(ch);
    return uc>='0'&&uc<='9';
}
bool is_valid_char(char const ch){
    auto uc=static_cast<unsigned char>(ch);
    return ::is_letter(uc)||::is_number(uc)||uc=='\''||uc=='-';
}
bool is_valid_word(::std::string const& word){
    bool has_letter_or_number=false;
    for(char ch:word){
        if(::is_letter(ch)||::is_number(ch)){
            has_letter_or_number=true;
        }
    }
    return has_letter_or_number;
}
// 标准化单词(转换为小写)
void normalize_word(::std::string& word){
    for(char& ch:word){
        if(ch>='A'&&ch<='Z'){
            ch=ch-'A'+'a';
        }
    }
}
// 文本处理导入数据库
void handle(::std::string const& text){
    ::std::string word={};
#define INSERT_WORD do{\
    if(::is_valid_word(word)){ \
        ::normalize_word(word); \
        if(::data_base.find(word)==::data_base.end()){ \
            ::data_base[word]=1; \
        }else{ \
            ::data_base[word]++; \
        } \
    } \
    word.clear(); \
}while(0) \
//
    for(char ch:text){
        if(::is_valid_char(ch)){
            word.push_back(ch);
        }else{
            INSERT_WORD;
        }
    }
    // 处理文本结束时的最后一个单词
    INSERT_WORD;
#undef INSERT_WORD
}
// 生成按频率排序的输出
void generate_output(void){
    ::output.clear();
    for(const auto& [word, freq]: ::data_base){
        // multimap会自动按key排序,使用greater使频率高的在前
        ::output.insert({freq,word});
    }
}
// 显示统计结果
void display_statistics(void){
    if(::data_base.empty()){
        std::cout<<"Database is empty!\n";
        return;
    }
    std::cout<<"\n=== WORD FREQUENCY ANALYSIS RESULTS ===\n";
    std::cout<<"Total unique words: "<<::data_base.size()<<'\n';
    // 计算总单词数
    size_t total_words=0;
    for(const auto& pair: ::data_base){
        total_words+=pair.second;
    }
    std::cout<<"Total word occurrences: "<<total_words<<'\n';
    // 使用multimap输出(已按频率排序)
    std::cout<<"\nWORD FREQUENCY RANKING(descending order):\n";
    std::cout<<"RANK | WORD | FREQUENCY | PERCENTAGE\n";
    std::cout<<"--------------------------------\n";
    size_t rank=1;
    for(const auto& [freq,word]: ::output){
        double percentage=(static_cast<double>(freq)/total_words)*100.0;
        std::cout<<rank++<<" | "
                 <<word<<" | "
                 <<freq<<" | "
                 <<percentage<<"%\n";
    }
}
// 读取文本文件
::std::string read_file(::std::string const& file_path){
    ::std::ifstream file(file_path);
    if(!file.is_open()){
        throw ::std::runtime_error("Can't open file:"+file_path);
    }
    ::std::stringstream buffer;
    buffer<<file.rdbuf();
    file.close();
    return buffer.str();
}
int main(int argc,char* argv[]){
    if(argc!=2){
        ::std::cout<<"use: <file path>\n";
        return -1;
    }
    ::handle(::read_file(argv[1]));
    ::generate_output();
    ::display_statistics();
    return 0;
}
