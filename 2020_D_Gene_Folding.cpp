#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

//prepossessing of a string to compute the length of even palindromes
string preString(string& str){
    string preStr = "#";    
    for(char c : str){
        preStr += c;
        preStr += '#';
    }    
    return preStr;
}

//computes the palindrome length for each char as center of the palindrome
vector<int> manacherAlgorithm(string& str){
    vector<int> lengths(str.length(),0);
    int c=0, r=0;
    for(int i=0; i<str.length(); i++){
        int mirror = 2*c-i;
        if(i<r){
            lengths[i] = min(r-i, lengths[mirror]);
        }
        int right = i+(1+lengths[i]);
        int left = i-(1+lengths[i]);
        while(left>=0 && right<str.length() && str[right] == str[left]){
            lengths[i]++;
            right++;
            left--;
        }
        if(i+lengths[i]>r){
            c = i;
            r = i+lengths[i];
        }
    }
    return lengths;
}

//computes the maximum prefix-palindrome index
int maxCut(vector<int>& lengths, string& str){
    int max=0;
    vector<int> lengths2 = lengths;
    for(int i=2; i<=(lengths2.size()); i+=2){
        if((lengths2[i]>0 && lengths2[i] == i) || lengths2[i]+max >= i){
            max = i;
        }
    }
    return max;
}

int main()
{   
    string input;
    cin >> input;
    
    //prefix
    string preStr = preString(input);
    vector<int> result = manacherAlgorithm(preStr);
    int prefixMax = maxCut(result, input);

    //suffix
    preStr = preStr.substr(prefixMax, preStr.length());
    vector<int> result2 = manacherAlgorithm(preStr);
    reverse(result2.begin(), result2.end());
    input = input.substr(prefixMax/2,input.length()-1);
    int suffixMax = prefixMax + result2.size()-maxCut(result2, input)-1;

    //computes the chars of the remaining string
    int res = ((suffixMax)-(prefixMax))/2;
    cout << res << endl;
    return 0;
}