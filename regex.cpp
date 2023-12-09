#include <iostream>
#include <vector>
#include <regex>
using namespace std;

class node{
public:
  string name; // state node name

  bool transited; // for syncronization
  bool state; // 0: deactivated, 1: activated
  bool isTerminal; // 0: terminal, 1: non-terminal
  
  char match; // target character for matching


  vector<node*> next; // next link 

public:

    void changeIsTerminal(){
        isTerminal = true;
    }

    void init(const string &_name, bool _isTerminal, char _match) {
    name = _name;

    isTerminal = _isTerminal;
    match = _match;

    state = transited = 0;
  }

  void addNode(node *_next){
    next.push_back(_next);
  }
 
  // for syncronization
  node& transition() { 
    transited = 1; 
    return *this;
  }
  void activation(){
    state = transited;
    transited = 0;
  }

  void input(const char &ch){
/*
      cout << "------------" << endl;
      cout << "state: " << state << endl;
      cout << "match: " << match << endl;
      cout << "ch: " << ch << endl;
      cout << "transited: " << transited << endl;
      cout << "------------" << endl;

*/

      if(state && (match == ch || (match == '.'))){
        if(isTerminal){
            cout << "  >>>> accepted by " << match << endl;
        }

        for(int i = 0 ; i < next.size(); i++){
            next[i]->transition();
            //cout << next[i] << next[i] ->name << next[i]->match << next[i]-> transited << endl;
        }
    }

      if (!next.empty() &&( (next[0]->match != ((&ch)+1)[0] || (next[0]->match == '\0' && ((&ch)+1)[0] == '\0') )&& ( state && (next.back()-> transited && (next.back()->match == '\0'))))){

          cout << "  >>>> accepted by " << match  << endl;
       }

    state = 0; // deactivate this node after matching
  }
};

void test(vector<node*> &s, const string &str){
  cout << "test for ' " << str << "'" << endl;

  for(int i = 0 ; i < str.length() ; i++){
    cout<< "  >> input " << str[i] << endl;
    
    // epsilon activation for s0
    s[0]->transition().activation();
   
    // give ch all nodes
    for(int j = 0 ; j < s.size(); j++){
        s[j]->input( str[i] );
       /* if (!s[j]->next.empty() && ( (s[j]->next.back()-> transited && (s[j]->next.back() -> match == '\0')))){
            cout << "  >>>> accepted by1 " << s[j]->match  << endl;
        };   */
    }

    for(int j = 0 ; j < s.size(); j++) {
        s[j]->activation();
    }
  }
  cout << endl;
}

vector<node*> makeRegex(const string& str) {
    vector<node*> s;
    for (int i = 0; i < str.length(); i++) {
        node* newNode = new node();
        string nodeName = "s" + to_string(s.size());
        bool isTerminal = (i == str.length() - 1);
        char matchChar = str[i];


        if (matchChar == '*' && !s.empty()) {
            node* prevNode = s.back();
            prevNode->addNode(prevNode);
            if (i < str.length() - 1) {
                i++;
                node* nextNode = new node();
                string nextNodeName = "s" + to_string(i);
                bool nextIsTerminal = (i == str.length() - 1);
                nextNode->init(nextNodeName, nextIsTerminal, str[i]);
                prevNode->addNode(nextNode);
                s[i-3]->addNode(nextNode);
                s.push_back(nextNode);
            } else {
                newNode->init("s" + to_string(i), true,'\0');
                s.push_back(newNode);
                prevNode->addNode(newNode);
                auto it = s.end();
                it--;
                it--;
                it--;
                (*it)->addNode(newNode);

            }
        } else if (matchChar == '+' && !s.empty()) {
            node* prevNode = s.back();
            prevNode ->addNode(prevNode);
            if (i < str.length() -1) {
                i++;
                node* nextNode = new node;
                string nextNodeName = "s" + to_string(i);
                bool nextIsTerminal = (i == str.length() -1);
                nextNode ->init(nextNodeName,nextIsTerminal,str[i]);
                prevNode->addNode(nextNode);
                s.push_back(nextNode);
            } else{
                newNode->init("s" + to_string(i), true,'\0');
                s.push_back(newNode);
                prevNode->addNode(newNode);
            }
        } else if (matchChar == '{' && !s.empty()) {

            string range = str.substr(i+1,str.find('}',i) - i - 1);
            int comma = range.find(',');
            int min = stoi(range.substr(0,comma));
            int max = stoi(range.substr(comma+1));

            for (int j = 0; j < max - 1; j++) {
                node* prevNode = s.back();
                node* newNode = new node();
                newNode->init("s" + to_string(s.size()), false,prevNode->match);
                prevNode->addNode(newNode);
                s.push_back(newNode);
            }

            node* nextNode = new node();
            int nextPos = str.find('}',i);
            if (nextPos < str.length() - 1){
                nextPos++;
                nextNode->init("s"+ to_string(s.size()),(nextPos == str.length() -1),str[nextPos]);
                s.push_back(nextNode);
                i =  nextPos;
            } else {
                nextNode->init("s"+ to_string(s.size()), true,'\0');
                s.push_back(nextNode);
                i =  nextPos;
            }

            auto it = s.end();
            it--;
            it--;
            for (int j = 0; j < (max - min) + 1 ; j++) {
                (*it)->addNode(nextNode);
                it--;
            }

        } else if ( matchChar == '|' && !s.empty()){
            node* prevNode = s.back();
            i++;
            if (i <str.length() -1){
                node* newNode = new node();
                newNode->init("s"+ to_string(s.size()), false,str[i]);
                s.push_back(newNode);
                auto it = s.end();
                it--;
                it--;
                it--;
                (*it)->addNode(newNode);
                i++;
                node* nextNode = new node();
                nextNode->init("s"+ to_string(s.size()),i == str.length() -1,str[i]);
                s.push_back(nextNode);
                newNode->addNode(nextNode);
                prevNode->addNode(nextNode);

            } else {
                node* newNode = new node();
                newNode->init("s" + to_string(s.size()),true,str[i]);
                s.push_back(newNode);
                auto it = s.end();
                it--;
                it--;
                it--;
                (*it)->addNode(newNode);
                prevNode->changeIsTerminal();


            }

        }
        else {
            newNode->init(nodeName, isTerminal, matchChar);
            s.push_back(newNode);
            if (i != 0) {
                auto it = s.end();
                it--;
                it--;
                (*it)->addNode(newNode);
            }
        }
    }
    return s;
}




int main(){

    string test1 = "abc";

    vector<node*> s1 = makeRegex("abc*");
    test(s1,test1);







}

