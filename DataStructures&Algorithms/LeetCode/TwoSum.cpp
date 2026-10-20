class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        vector<int> n;
        for (int i = 0; i < nums.size(); i++) {
            for(int j = 0; j < nums.size(); j++)
            if (nums[i] + nums[j] == target && i != j) {
                n = {i, j};
                return n;
            }
        }
        return n;
    }
};
