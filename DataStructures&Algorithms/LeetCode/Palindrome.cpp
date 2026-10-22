class Solution {
public:
    bool isPalindrome(int x) {
        std::string z = std::to_string(x);
        std::string y = z;
            std::reverse(z.begin(), z.end());
            if(z == y){
                return true;
            }
        
    return false;
    }
};
