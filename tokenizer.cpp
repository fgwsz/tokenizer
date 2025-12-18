#include<cstddef>       // ::std::size_t
#include<algorithm>     // ::std::sort
#include<string>        // ::std::string ::std::to_string
#include<array>         // ::std::array
#include<unordered_map> // ::std::unoredered_map
#include<vector>        // ::std::vector:
#include<iostream>      // ::std::cout
#include<fstream>       // ::std::ifstream
#include<sstream>       // ::std::stringstream
#include<stdexcept>     // ::std::runtime_error

// 单词频率数据库
static ::std::unordered_map<::std::string,::std::size_t> data_base={};

inline void handle(::std::string const& text){
    //静态查找表
    static constexpr ::std::array<unsigned char,256> table=[](void){
        ::std::array<unsigned char,256> table={};
        for(::std::size_t index=0;index<256;++index){
            if(index>='A'&&index<='Z'){
                table[index]=1;
            }else if((index>='a'&&index<='z')||(index>='0'&&index<='9')){
                table[index]=2;
            }else if(index=='\''||index=='-'){
                table[index]=3;
            }
        }
        return table;
    }();
    ::std::string word={};
    word.reserve(64);//预分配最大英文单词长度
    bool word_has_letter_or_number=false;
    //一次性遍历并处理完所有单词操作导入数据库
    for(unsigned char ch:text){
        switch(table[ch]){
            case 1:{//大写字母
                word.push_back(ch+'a'-'A');
                word_has_letter_or_number=true;
                break;
            }
            case 2:{//小写字母或数字
                word.push_back(ch);
                word_has_letter_or_number=true;
                break;
            }
            case 3:{//缩写符号或连字符号
                word.push_back(ch);
                break;
            }
            default:{//遇到分隔符
                if(word_has_letter_or_number){
                    auto[iter,inserted]=::data_base.try_emplace(word,1);
                    if(!inserted){
                        ++iter->second;
                    }
                }
                word.clear();
                word_has_letter_or_number=false;
            }
        }
    }
    // 处理文本结束时的最后一个单词
    if(word_has_letter_or_number){
        auto[iter,inserted]=::data_base.try_emplace(word,1);
        if(!inserted){
            ++iter->second;
        }
    }
}
// 得到打印时占用的字符宽度
inline ::std::size_t get_digit_width(::std::size_t value){
    return ::std::to_string(value).size();
}
// 打印时的对齐方式
enum class Alignment{
    LEFT,
    RIGHT
};
// 数值型数据打印时拓展到指定的宽度
template<typename Number>
inline ::std::string expand_digit_width(
    Number value
    ,::std::size_t width
    ,::Alignment alignment
){
    ::std::string str=::std::to_string(value);
    ::std::string blank=::std::string(width-str.size(),' ');
    if(alignment==::Alignment::RIGHT){
        return blank+str;
    }else{
        return str+blank;
    }
}
// 显示统计结果
inline void display(void){
    struct Item{
        ::std::size_t freq;
        ::std::string_view word;
    };
    static ::std::vector<Item> output={};
    if(::data_base.empty()){
        ::std::cout<<"Database is empty!\n";
        return;
    }
    ::std::cout<<
        "\n=== WORD FREQUENCY ANALYSIS RESULTS ===\n"
        "Total unique words: "
        <<::data_base.size()<<'\n';
    // 计算总单词数
    ::std::size_t total_words=0;
    for(const auto& pair: ::data_base){
        total_words+=pair.second;
    }
    ::std::cout<<"Total word occurrences: "<<total_words<<'\n';
    // 按频率排序
    output.clear();
    for(const auto& [word,freq]: ::data_base){
        output.emplace_back(Item{freq,word});
    }
    ::std::sort(output.begin(),output.end(),
        [](Item const& lhs,Item const& rhs){
            if(lhs.freq!=rhs.freq){
                return lhs.freq>rhs.freq;
            }else{
                //Item降序排列时,单词按字母a-z升序排列
                return lhs.word<=rhs.word;
            }
        }
    );
    ::std::cout<<
        "\nWORD FREQUENCY RANKING(descending order):\n"
        "RANK | FREQUENCY | PERCENTAGE | WORD\n"
        "--------------------------------\n";
    ::std::size_t rank=1;
    ::std::size_t max_digit_width=get_digit_width(total_words);
    double percentage=0.0;
    for(const auto& [freq,word]:output){
        percentage=(static_cast<double>(freq)/total_words)*100.0;
        ::std::cout
            <<expand_digit_width(rank,max_digit_width,::Alignment::LEFT)<<"|"
            <<expand_digit_width(freq,max_digit_width,::Alignment::RIGHT)<<"|"
            <<expand_digit_width(percentage,10,::Alignment::RIGHT)<<" %| "
            <<word<<"\n";
        ++rank;
    }
}
// 读取文本文件
inline ::std::string read_file(::std::string const& file_path){
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
    if(argc<2){
        ::std::cout<<"use: <file path>...\n";
        return -1;
    }
    ::std::string text={};
    for(int index=1;index<argc;++index){
        text+=::read_file(argv[index])+'.';
    }
    ::handle(text);
    ::display();
    return 0;
}
