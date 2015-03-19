whitespaces [ \t]
newline [\n]
integerPart [0-9]+

%%

{whitespaces} {}
{newline} {lineNo++;}
"int" {return Parser::INT;}
"void" {return Parser::VOID;}
"float" {return Parser::FLOAT;}
"if" {return Parser::IF;}
"while" {return Parser::WHILE;}
"else" {return Parser::ELSE;}
"for" {return Parser::FOR;}
"return" {return Parser::RETURN;}
[\(\);,\{\}\[\]+*\-/<>] {return matched()[0];}
= {return Parser::ASSIGN_OP;}
\+\+ {Parser::d_val__ = std::string("PP");return Parser::INC_OP;}
! {return Parser::NOT_OP;}
\|\| {return Parser::OR_OP;}
&& {return Parser::AND_OP;}
== {return Parser::EQ_OP;}
!= {return Parser::NE_OP;}
\<= {return Parser::LE_OP;}
\>= {return Parser::GE_OP;}
([1-9][0-9]*)|(0[bB][01]+)|(0[0-7]+)|(0[xX][0-9a-fA-F]+)|0 {Parser::d_val__ = std::string(matched()); return Parser::INT_CONSTANT;}
([0-9]*\.[0-9]+([eE][\+-]?[0-9]+)?)|([0-9]+\.)|([0-9]+\.[eE][\+-]?[0-9]+) {Parser::d_val__ = std::string(matched());return Parser::FLOAT_CONSTANT;}
\"([^\n\"]*\\\")*[^\n\"]*\" {Parser::d_val__ = std::string(matched());return Parser::STRING_LITERAL;}
[[:alpha:]_][[:alnum:]_]* {Parser::d_val__ = std::string(matched());return Parser::IDENTIFIER;}
. {}
