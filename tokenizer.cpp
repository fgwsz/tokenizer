#include<cstddef>       // ::std::size_t
#include<algorithm>     // ::std::sort
#include<string>        // ::std::string ::std::to_string
#include<array>         // ::std::array
#include<unordered_map> // ::std::unoredered_map
#include<vector>        // ::std::vector:
#include<iostream>      // ::std::cout ::std::cin  ::std::ios_base ::std::ios
#include<fstream>       // ::std::ifstream ::std::ofstream
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
            }else if(index=='\''||index=='-'||index=='_'){
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
// 返回统计结果
inline ::std::string display(void){
    struct Item{
        ::std::size_t freq;
        ::std::string_view word;
    };
    ::std::string ret={};
    if(::data_base.empty()){
        ret.append("Database is empty!\n");
        return ret;
    }
    ret.append(
        "\n=== WORD FREQUENCY ANALYSIS RESULTS ===\n"
        "Total unique words: "+::std::to_string(::data_base.size())+'\n'
    );
    // 计算总单词数
    ::std::size_t total_words=0;
    for(const auto& pair: ::data_base){
        total_words+=pair.second;
    }
    ::std::string total_words_string=::std::to_string(total_words);
    ret.append("Total word occurrences: "+total_words_string+'\n');
    // 按频率排序
    ::std::vector<Item> output={};
    output.reserve(::data_base.size());//预分配内存
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
    ret.append(
        "\nWORD FREQUENCY RANKING(descending order):\n"
        "RANK | FREQUENCY | PERCENTAGE | WORD\n"
        "--------------------------------\n"
    );
    ::std::size_t rank=1;
    ::std::size_t max_digit_width=total_words_string.size();
    double percentage=0.0;
    for(const auto& [freq,word]:output){
        percentage=(static_cast<double>(freq)/total_words)*100.0;
        ret.append( 
            ::expand_digit_width(rank,max_digit_width,::Alignment::LEFT)+"|"
            +::expand_digit_width(freq,max_digit_width,::Alignment::RIGHT)+"|"
            +::expand_digit_width(percentage,10,::Alignment::RIGHT)+" %| "
        );
        ret.append(word).append("\n");
        ++rank;
    }
    return ret;
}
// 读取文本文件
inline ::std::string read_file(::std::string const& file_path){
    ::std::ifstream file(file_path,::std::ios::ate);// 直接定位到文件末尾
    if(!file.is_open()){
        throw ::std::runtime_error("Can't open file:"+file_path);
    }
    ::std::size_t file_size=file.tellg();
    file.seekg(0);
    ::std::string content;
    content.resize(file_size);
    file.read(&content[0],file_size);
    return content;
}
int main(int argc,char* argv[]){
    static bool std_cout_init=[](void){
        //关闭与C语言的输入输出流同步
        ::std::ios_base::sync_with_stdio(false);
        //解除cin和cout的绑定
        ::std::cin.tie(nullptr);
        ::std::cout.tie(nullptr);
        return true;
    }();
    auto help=[](void){
        ::std::cout<<
            "use: <input file path>\n"
            "use: <input file path>...\n"
            "use: <input file path> -o <output file path>\n"
            "use: <input file path>... -o <output file path>\n";
    };
    if(argc<2){
        help();
        return -1;
    }
    ::std::string text={};
    ::std::string argument={};
    ::std::string output={};
    enum class Mode{
        COUT,
        FOUT
    };
    Mode mode=Mode::COUT;
    for(int index=1;index<argc;++index){
        argument=argv[index];
        if(argument=="-o"){
            mode=Mode::FOUT;
            //检查输出路径的数量是否为一个,多于一个显示错误
            if(index>1&&(index+2==argc)){
                output=argv[index+1];
                break;
            }else{
                help();
                return -1;
            }
        }else{
            text+=::read_file(argument)+'.';
        }
    }
    ::handle(text);
    if(mode==Mode::COUT){
        ::std::cout<<::display();
    }else{
        ::std::ofstream ofs(output);
        if(!ofs.is_open()){
            throw ::std::runtime_error("Can't open file:"+output);
            return -1;
        }
        ofs<<::display();
        ofs.close();
    }
    return 0;
}
