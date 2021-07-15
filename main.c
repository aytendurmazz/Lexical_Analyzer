#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int isInteger(FILE* file,char c,FILE* wfile){
    int current=0;
    char list[10]={'0'}; //Array for a maximum 10 digit integer

    while(isalnum(c)>0 || (ispunct(c)>0 && c!=';' && c!='(' && c!=')' && c!='[' && c!=']'&& c!='{' && c!='}') ){ //Control of an invalid type(identifier)(For example:"2&str" or " 25(" )
        list[current++]=c;
        c=fgetc(file);
    }

    for(int i = 0 ; i<strlen(list);i++){ //Control of an invalid type
        if(isalpha(list[i])){
            fprintf(wfile,"ERROR!INVALID TYPE \n");
            ungetc(c,file); //Moved the pointer to one character left,to control end of line
            return 0 ;
        }
    }

    if(current<10){
        fprintf(wfile,"IntConst(%s)\n",list);  //Writing to the file
    }else{
        fprintf(wfile,"ERROR!Integer size cannot be greater than 10 digit!\n");
        ungetc(c,file); //Moved the pointer to one character left,to control end of line
        return 0;
    }
    ungetc(c,file);
}
int isKeyword(char list1[]){
    char keywords[18][10] ={"break","case","char","const","continue","do","else","enum",
                          "float","for","goto","if","int", "long","record","return","static","while"}  ;
    for(int j=0;j<strlen(list1);j++){
        list1[j]=tolower(list1[j]);
    }

    for(int i=0;i<18;++i){
        if(strcmp(keywords[i], list1) == 0) //Control of equation
        {
            return 1;
        }
    }
    for(int j=0;j<strlen(list1);j++){
        list1[j]=toupper(list1[j]);
    }

return 0;
}
int isIdentifier(FILE* file,char c,FILE* wfile){
    char list1[20]={'0'};
    int current=0;

    while(isalnum(c)>0 || (ispunct(c)>0 && c!=';' && c!='+' && c!='-' && c!=':' && c!='*' && c!='/' && c!='(' && c!=')' && c!='[' && c!=']'&& c!='{' && c!='}' ) ){//Appending the identifier to array

        list1[current++]=toupper(c);
        c=fgetc(file);


    }
    ungetc(c,file);
    for(int i = 0 ; i<strlen(list1);i++){ //Control of an invalid type
        if(ispunct(list1[i]) && list1[i]!='_'){
            fprintf(wfile,"ERROR!INVALID TYPE \n");
            return 0 ;
        }
    }

    if(isKeyword(list1)){

        fprintf(wfile,"Keyword(%s)\n",list1); //Writing to the file
    }else{
        if(current<20){
            fprintf(wfile,"Identifier(%s)\n",list1);
        }else{
            fprintf(wfile,"ERROR!Identifier size must be maximum 20 characters!\n");
            return 0;
        }
    }
}
int isComment(FILE* file,char c ,FILE* wfile){
    if(c=='*'){
        c=fgetc(file);

        while(c!='*'){

            c=fgetc(file);

            if(c=='\n' || c==EOF || c==';'){  //Controlling if the comment terminates before the file end
                fprintf(wfile,"ERROR!Comment Incomplete\n");
                ungetc(c,file); //Moved the pointer to one character left,to continue reading file
                return 0;
             }
        }

        c=fgetc(file);
        if(c!=')'){
            fprintf(wfile,"ERROR!Comment Incomplete!\n");
            ungetc(c,file); //Moved the pointer to one character left,to continue reading file
            return 0;
        }
        else{

            return 1;
        }

    }else{
        ungetc(c,file); //Moved the pointer to one character left,to continue reading file
    }
}
int isBrackets(FILE* file,char c,FILE* wfile){
    char brackets[6] ={'[',']','(',')','{','}'}  ;
    for(int a=0;a<6;a++){
        if(c==brackets[a]){

            if(c=='{'){
                fprintf(wfile,"LeftCurlyBracket\n"); }
            else if(c=='}'){
                fprintf(wfile,"RightCurlyBracket\n"); }
            else if(c=='['){
                fprintf(wfile,"LeftSquareBracket\n"); }
            else if(c==']'){
                fprintf(wfile,"RightSquareBracket\n"); }
            else if(c=='('){ //If the character starts with '(',checks whether it is a comment or not
                c=fgetc(file);
                if(c=='*'){
                    isComment(file,c,wfile);
                }else{
                    ungetc(c,file); //Moved the pointer to one character left,to control the left paranthesis situation
                    fprintf(wfile,"LeftPar\n");
                }
            }
            else if(c==')'){
                fprintf(wfile,"RightPar\n"); }
        return 1;
        }
    }
}
void isOperator(FILE* file,char c,FILE* wfile){
    char operators[7][3] ={"++","-","*","/","+","--",":="} ;
    char list2[3]={'0'};

    list2[0]=c;
    c=fgetc(file);

    if((c=='+' ||c=='-' || c=='=')  && c!=';' ){ //Array for a maximum 2 characters operator
        list2[1]=c;
    }

    for(int i=0;i<7;++i){
        if(strcmp(operators[i], list2) == 0) //Control of equation
        {
            fprintf(wfile,"Operator(%s)\n",list2);
        }
    }

    if(strlen(list2)!=2){ //If the operator size is 1,don't read other character in this block.
        ungetc(c,file);
    }
}
int isEndofLine(char c , FILE* wfile){
    if(c == ';'){
        fprintf(wfile,"EndOfLine\n");
        return 1;
    }
    else{
        return 0;
    }
}
void isString(FILE* file,char c,FILE* wfile,int len){
     int i=0;
     char *list3;//Creating a dynamic array using malloc function
     list3 = (char*) malloc(sizeof(char)*len);

    if(c=='"'){ //" character is array's first index element
        c=fgetc(file);
    }
    while(c!='"') { //While other character is not a " ,keep adding the characters to array
        list3[i++]=c;
        c=fgetc(file);

        if(c=='\n' || c==EOF ||c==';'){  //Controlling if a string constant terminates before the file end
            fprintf(wfile,"ERROR!String Incomplete!\n");
            isEndofLine(c,wfile);
            return 0 ;
        }
    }
    fprintf(wfile,"String(%s)\n",list3);
    free(list3);
}
int main(){
    FILE* file;
    FILE* wfile;//File to write
    wfile=fopen("code.lex.txt","w");
    int len=0;

    char c;
    if((file=fopen("code.psi.txt","r"))!= NULL)
    {
        fseek(file,0,SEEK_END); //For dynamic array
        len=ftell(file);
        rewind(file);

        while((c = fgetc(file)) != EOF)
        {

            if(isalpha(c)>0){
                isIdentifier(file,c,wfile);
            }
            else if(isdigit(c)>0){
                isInteger(file,c,wfile);
            }
            else if(ispunct(c)>0 ){
                 if (isBrackets(file,c,wfile)==1) { continue;}
                 isOperator(file,c,wfile);
                 isEndofLine(c,wfile);
                 if(c=='"'){isString(file,c,wfile,len);}

            }


       }
       fclose(file);
    }
    else{
        fprintf(wfile,"FILE NOT FOUND");

    }

    return 0;
}
