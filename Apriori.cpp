#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include<chrono>
#include<iomanip>
using namespace std;
struct Item {
    vector<int> item;
    double Supportlevel;
};
vector<vector<int> > ReadDateSetFromFile(const string& filename) {
    vector<vector<int> > res;
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            vector<int> path;
            stringstream sline(line);
            int temp;
            while (sline >> temp) {
                path.push_back(temp);
            }
            res.push_back(path);
        }
        file.close();
    } else {
        cerr << "The file can't be open: " << filename << endl;
    }
    return res;
}
vector<vector<int> >fun(vector<int>&item)
{
    vector<vector<int> >res;
    for(int i=0;i<item.size();i++)
    {
        vector<int>sub(item);
        sub.erase(sub.begin()+i);
        res.push_back(sub);
    }
    return res;
}
bool checkSubsets(const vector<Item>& frequentItems,  vector<int>& item, int k) {
    vector<vector<int> >candidates=fun(item);

 for(int i=0;i<candidates.size();i++)
    {
        bool flag=true;
        for(int j=0;j<frequentItems.size();j++)
        {
            if(frequentItems[j].item.size()==k-1)
            {
                
                if(!includes(frequentItems[j].item.begin(), frequentItems[j].item.end(), candidates[i].begin(), candidates[i].end()))
                {
                    flag=false;
                    break;
                }
            }
        }
        if(!flag) return false;
    }

    // 所有子集都在频繁项集中，返回 true
    return true;
}

vector<Item> generateCandidates(const vector<Item>& frequentItems, int k) {
   // cout<<1<<endl;
    vector<Item> res;
    for (int i = 0; i < frequentItems.size(); i++) {
        for (int j = i + 1; j < frequentItems.size(); j++) {
            // 检查两个项集的前 k-1 个元素是否相同
            if (frequentItems[i].item.size() == k  && frequentItems[j].item.size() == k ) {
                bool equal = true;
                for (int m = 0; m < k - 1; m++) {
                    if (frequentItems[i].item[m] != frequentItems[j].item[m]) {
                        equal = false;
                        break;
                    }
                }
                if (equal) {
                    // 合并两个频繁项集
                    Item temp(frequentItems[i]);
                    //temp.item.assign(frequentItems[i].item.begin(), frequentItems[i].item.end());
                    temp.item.push_back(frequentItems[j].item.back());
                    if(checkSubsets(frequentItems,temp.item,k))//剪枝
                        res.push_back(temp);
                }
                else break;
            }
        }
    }
    return res;
}
double calculateSupport(const vector<vector<int> >& dateset, const Item& its) {
    int count = 0;
    for (int i = 0; i < dateset.size(); i++) {
        bool flag = true;
        for (int j = 0; j < its.item.size(); j++) {
            if (find(dateset[i].begin(), dateset[i].end(), its.item[j]) == dateset[i].end()) {
                flag = false;
                break;
            }
        }
        if (flag) {
            count++;
        }
    }
    return count;
}
bool compareVectors(const Item& v1, const Item& v2) {
    for(int i=0;i<min(v1.item.size(),v2.item.size());i++)
    {
        if(v1.item[i]>v2.item[i]){
            return false;
        }
        else if(v1.item[i]<v2.item[i])return true;
    }
  return v1.item.size() < v2.item.size();
}
vector<Item> Apriori(vector<vector<int> >& dateset, double SupportLevel) {
    vector<Item> res;
    // 找到所有单项的频繁项集
    unordered_map<int, int> itemcount;
    for (int i = 0; i < dateset.size(); i++) {
        for (int j = 0; j < dateset[i].size(); j++) {
            itemcount[dateset[i][j]]++;
        }
    }
    for (auto it = itemcount.begin(); it != itemcount.end(); ++it) {
        if (it->second / (double)dateset.size() >= SupportLevel) {
            Item itemset;
            itemset.item.push_back(it->first);
            itemset.Supportlevel = it->second;
            res.push_back(itemset);
        }
    }
    sort(res.begin(), res.end(), compareVectors);
    // 生成二项集
    //cout<<res.size()<<endl;
   //int sum=0;
    vector<Item >candidates;
    for(int i=0;i<res.size();i++){
        for(int j=i+1;j<res.size();j++){
           // cout<<sum++<<" "<<1<<endl;
            Item items;
            items.item.push_back(res[i].item[0]);
            items.item.push_back(res[j].item[0]);
            candidates.push_back(items);
        }

    }
    for(int i=0;i<candidates.size();i++)
    {
       // cout<<sum++<<endl;
        candidates[i].Supportlevel=calculateSupport(dateset,candidates[i]);
    }
    vector<Item>newFrequentItem;
    for(int i=0;i<candidates.size();i++){
        if(candidates[i].Supportlevel/dateset.size()*1.0>=SupportLevel){
            newFrequentItem.push_back(candidates[i]);
        }
    }
    for(int i=0;i<newFrequentItem.size();i++){
      //  cout<<sum++<<endl;
        res.push_back(newFrequentItem[i]);
    }
    int k = 2;
    while (true) {
        //cout<<1<<endl;
       // cout<<1<<endl;
       sort(newFrequentItem.begin(),newFrequentItem.end(),compareVectors);
        vector<Item> result = generateCandidates(newFrequentItem, k);//获得候选项集
        // 计算每个候选项集的支持度
        newFrequentItem.clear();
       // cout<<"result.size()="<<result.size()<<endl;;
       
        for (int i = 0; i < result.size(); i++) {
            result[i].Supportlevel = calculateSupport(dateset, result[i]);
           // cout<<i<<": "<<result[i].Supportlevel<<endl;
        }

        //vector<Item> newFrequentItem;
        
        for (int i = 0; i < result.size(); i++) {
            if (result[i].Supportlevel / (double)dateset.size() >= SupportLevel) {
                newFrequentItem.push_back(result[i]);
            }
        }
        // 如果没有找到新的频繁项集，则结束循环
        if (newFrequentItem.empty()) {
            break;
        }
        // 更新频繁项集列表
        for (int i = 0; i < newFrequentItem.size(); i++) {
            res.push_back(newFrequentItem[i]);
        }
        k++;
    }
    //sort(res.begin(), res.end(), cmp);
    return res;
}

int main() {
    // 获取最小支持度
    
    double SupportLevel;
    cin >> SupportLevel;
    // 读取数据集
    string filename = "/Users/wanglinhao/Desktop/retail.txt";
    vector<vector<int> > dateset = ReadDateSetFromFile(filename);

    // Apriori算法
    vector<Item> FrequentItem = Apriori(dateset, SupportLevel);
   
    cout << "包括：" << endl;
    for (int i = 0; i < FrequentItem.size(); i++) {
        cout << "{";
        for (int j = 0; j < FrequentItem[i].item.size(); j++) {
            cout << FrequentItem[i].item[j];
            if (j < FrequentItem[i].item.size() - 1) {
                cout << ",";
            }
        }
        cout << "}" << endl;
    }
     cout << "频繁项集共有：" << FrequentItem.size() << "项" << endl;
    return 0;
}