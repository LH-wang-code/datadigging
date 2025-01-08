#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <map>
using namespace std;
const double SupportLevel = 0.01;
int size_d;
struct FPNode
{
    int count;
    int data;
    FPNode *father;
    FPNode *child;
    FPNode *next;
    FPNode *brother;
    FPNode()
    {
        data = 0;
        count = 0;
        father = nullptr;
        child = nullptr;
        next = nullptr;
        brother = nullptr;
    }
};
struct listNode
{
    int data;
    int count;
    FPNode *next;
};
struct Item
{
    int data;
    int count;
};
struct SItem
{
    vector<int> items;
    int count;
    SItem(vector<int> itemss, int count)
    {
        for (int i = 0; i < itemss.size(); i++)
        {
            this->items.push_back(itemss[i]);
        }
        this->count = count;
    }
    SItem(){
        count=0;
    }
};
vector<vector<int> > ReadDateSetFromFile(const string &filename)
{
    vector<vector<int> > res;
    ifstream file(filename);
    if (file.is_open())
    {
        string line;
        while (getline(file, line))
        {
            vector<int> path;
            stringstream sline(line);
            int temp;
            while (sline >> temp)
            {
                path.push_back(temp);
            }
            res.push_back(path);
        }
        file.close();
    }
    else
    {
        cerr << "The file can't be open: " << filename << endl;
    }
    return res;
}
bool cmp(listNode &a, listNode &b)
{
    return a.count > b.count;
}

vector<listNode> GetHeadList(vector<vector<int> > &dateset) // 获取头表
{
    vector<listNode> L1;

    // 首先找单项频繁项集
    map<int, int> itemcount;
    for (int i = 0; i < dateset.size(); i++)
    {
        for (int j = 0; j < dateset[i].size(); j++)
        {
            if (itemcount.find(dateset[i][j]) != itemcount.end())
                itemcount[dateset[i][j]]++;
            else
                itemcount[dateset[i][j]] = 1;
        }
    }
    //  vector<listNode>res;
    for (auto it = itemcount.begin(); it != itemcount.end(); it++)
    {
        if (it->second>= ceil(SupportLevel*size_d))
        {
            listNode x;
            x.data = it->first;
            x.count = it->second;
            x.next = nullptr;
            L1.push_back(x);
        }
    }
    sort(L1.begin(), L1.end(), cmp);
    /*
    for(int i=0;i<L1.size();i++)
    {
        cout<<L1[i].data<<" ";
    }
    cout<<endl;
    */
    return L1;
}
vector<listNode> GetHeadList(map<vector<int>,int>&dateset)
{
    map<int,int>itemcount;
    for(auto it=dateset.begin();it!=dateset.end();it++)
    {
        for(int i=0;i<it->first.size();i++)
        {
            if(itemcount.find(it->first[i])!=itemcount.end())
                itemcount[it->first[i]]+=it->second;
            else
                itemcount[it->first[i]]=it->second;
        }
    }
    vector<listNode>L1;
    for(auto it=itemcount.begin();it!=itemcount.end();it++)
    {
        if(it->second/(double)size_d>=SupportLevel)
        {
            listNode x;
            x.data=it->first;
            x.count=it->second;
            x.next=nullptr;
            L1.push_back(x);
        }
    }
    sort(L1.begin(),L1.end(),cmp);
    return L1;

}

vector<vector<int> > Get_FPfile(vector<vector<int> > &dateset, vector<listNode> &headlist) // 删除非频繁元素
{
    // cout<<1<<endl;
    vector<vector<int> > datesets(dateset);
    vector<vector<int> > res;
    // cout<<1<<endl; 初始化没有大问题
    for (int i = 0; i < datesets.size(); i++)
    {
        // auto j=date
        vector<int> temp;
        // cout<<1<<endl;//只打印了一个1，在内循环中出现错误
        for (int j = 0; j < headlist.size(); j++)
        {
            // cout<<1<<endl;//在这里应该是只访问了i==0的时候
            bool flag = false;
            for (int k = 0; k < datesets[i].size(); k++)
            {
                
                // 发现了两个问题，第一个是我headlist下标用的i，第二个是使用erase擦除时对datesets[i]的size造成变化，会影响for循环。
                //不知道第几版代码了，发现了更多的问题，这里没排序，需要改一下两个嵌套的顺序。。。
                
                if (headlist[j].data == datesets[i][k])
                {
                    temp.push_back(headlist[j].data);
                }
            }
        }
        //    sort(temp.begin(),temp.end(),cmp1);
        if(temp.size()>0)
            res.push_back(temp);
    }
    /*
    // cout<<1<<endl;
    for(int i=0;i<res.size();i++)
    {
        for(int j=0;j<res[i].size();j++)
        {
            cout<<res[i][j]<<" ";
        }
        cout<<endl;
    }
    */
    return res;
}
vector<SItem>Get_FPfile(map<vector<int>,int>& dateset,vector<listNode> & headlist)
{
    vector<SItem >datesets;
    vector<SItem >res;
    for(auto it=dateset.begin();it!=dateset.end();it++)
    {
        SItem temp(it->first,it->second);

        datesets.push_back(temp);
    }
    for(int i=0;i<datesets.size();i++)
    {
        SItem temp;
        for(int j=0;j<headlist.size();j++)
        {
            for(int k=0;k<datesets[i].items.size();k++)
            {
                if(headlist[j].data==datesets[i].items[k])
                {
                    temp.items.push_back(headlist[j].data);
                    temp.count=datesets[i].count;
                }
            }
        }
        
        if(temp.items.size()>0)
        
            res.push_back(temp);
    }
    return res;
}
void Linknext(FPNode *newnode, vector<listNode> &headlist)
{
    for (int i = 0; i < headlist.size(); i++)
    {
        if (headlist[i].data == newnode->data)
        {

            if (headlist[i].next == nullptr)
            {
                headlist[i].next = newnode;
            }
            else
            {
                FPNode *p1 = headlist[i].next;
                FPNode *p2 = headlist[i].next;
                while (p1)
                {
                    p2 = p1;
                    p1 = p1->next;
                }
                p2->next = newnode;
            }
            break;
        }
    }
}
FPNode *CreateFPtree(vector<vector<int> > &datesets, vector<listNode> &headlist)
{
    FPNode *head = new FPNode;
    FPNode *p = head;
    FPNode *q = head;
    int flag = 0;
    for (int i = 0; i < datesets.size(); i++)
    {
        p = head;
        int j = 0;
        while (j < datesets[i].size())
        {
            flag = 0;
            if (i == 0)
            {
                FPNode *newnode = new FPNode;
                newnode->count = 1;
                newnode->data = datesets[i][j];
                newnode->father = p;
                p->child = newnode;
                p = p->child;
                j++;
                for (int k = 0; k < headlist.size(); k++)
                {
                    if (headlist[k].data == newnode->data)
                    {
                        headlist[k].next = newnode;
                        break;
                    }
                }
            }
            else
            {
                p = p->child;
                while (p && j < datesets[i].size())
                {
                    if (p->data == datesets[i][j])
                    {
                        p->count++;
                        q = p;
                        p = p->child;
                        flag = 1;
                        j++;
                    }
                    else
                    {
                        q = p;
                        p = p->brother;
                        flag = 2;
                    }
                }
                if (flag == 1)
                {
                    while (j < datesets[i].size())
                    {
                        FPNode *newnode = new FPNode;
                        newnode->count = 1;
                        newnode->father = q;
                        q->child = newnode;
                        newnode->data = datesets[i][j];
                        q = q->child;
                        j++;
                        Linknext(newnode, headlist);
                    }
                }
                else if (flag == 2)
                {
                    FPNode *newnode = new FPNode;
                    newnode->count = 1;
                    newnode->data = datesets[i][j];
                    q->brother = newnode;
                    newnode->father = q->father;
                    q = q->brother;
                    j++;
                    Linknext(newnode, headlist);
                    while (j < datesets[i].size())
                    {
                        FPNode *newnode = new FPNode;
                        newnode->count = 1;
                        newnode->father = q;
                        q->child = newnode;
                        newnode->data = datesets[i][j];
                        q = q->child;
                        j++;
                        Linknext(newnode, headlist);
                    }
                }
            }
        }
    }
    //if(true);
    return head;
}
map<vector<int>,int > GetFrequentItems(listNode &rt, FPNode *&head)
{
    // cout<<1<<endl;
    FPNode *p = rt.next;
    map<vector<int>,int > path;
    // cout<<p->data;
    //vector<int>pa;
    //pa.push_back(rt.data);
    // 一直找到根节点是一组。
    while (p)
    {
        // cout<<"hello world"<<endl;
        FPNode *q = p;
        int n=q->count;
        vector<int> pa;
        while (q->father != head)
        {
            q = q->father;
            pa.push_back(q->data);
            
            // 调试到这发生断错误
        }
        reverse(pa.begin(), pa.end());
        path[pa]=n;
        pa.clear();
        p = p->next;
    }
    return path;
}
FPNode * CreateFPtree(vector<SItem>&datesets,vector<listNode>&headlist)
{
 FPNode *head = new FPNode;
    FPNode *p = head;
    FPNode *q = head;
    int flag = 0;
    for (int i = 0; i < datesets.size(); i++)
    {
        p = head;
        int j = 0;
        while (j < datesets[i].items.size())
        {
            flag = 0;
            if (i == 0)
            {
                FPNode *newnode = new FPNode;
                newnode->count = datesets[i].count;
                newnode->data = datesets[i].items[j];
                newnode->father = p;
                p->child = newnode;
                p = p->child;
                j++;
                for (int k = 0; k < headlist.size(); k++)
                {
                    if (headlist[k].data == newnode->data)
                    {
                        headlist[k].next = newnode;
                        break;
                    }
                }
            }
            else
            {
                p = p->child;
                while (p && j < datesets[i].items.size())
                {
                    if (p->data == datesets[i].items[j])
                    {
                        p->count+=datesets[i].count;
                        q = p;
                        p = p->child;
                        flag = 1;
                        j++;
                    }
                    else
                    {
                        q = p;
                        p = p->brother;
                        flag = 2;
                    }
                }
                if (flag == 1)
                {
                    while (j < datesets[i].items.size())
                    {
                        FPNode *newnode = new FPNode;
                        newnode->count = datesets[i].count;
                        newnode->father = q;
                        q->child = newnode;
                        newnode->data = datesets[i].items[j];
                        q = q->child;
                        j++;
                        Linknext(newnode, headlist);
                    }
                }
                else if (flag == 2)
                {
                    FPNode *newnode = new FPNode;
                    newnode->count = datesets[i].count;
                    newnode->data = datesets[i].items[j];
                    q->brother = newnode;
                    newnode->father = q->father;
                    q = q->brother;
                    j++;
                    Linknext(newnode, headlist);
                    while (j < datesets[i].items.size())
                    {
                        FPNode *newnode = new FPNode;
                        newnode->count = datesets[i].count;
                        newnode->father = q;
                        q->child = newnode;
                        newnode->data = datesets[i].items[j];
                        q = q->child;
                        j++;
                        Linknext(newnode, headlist);
                    }
                }
            }
        }
    }
    //if(true);
    return head;
}
vector<SItem> result;
void Getresult(vector<listNode> &headlist, FPNode *head, vector<int> &base)
{
    // cout<<1<<endl;
    if (headlist.empty())
        return;

    for (int i = headlist.size() - 1; i >= 0; i--)
    {
        // cout<<1<<endl;
        // cout<<1<<endl;
        SItem item(base, headlist[i].count);
        // item.data=headlist[i]->data;
        item.items.push_back(headlist[i].data);
       // base.push_back(headlist[i].data);
        result.push_back(item);
        // cout<<1<<endl;
        map<vector<int>,int> frequentitems = GetFrequentItems(headlist[i], head); // 获取在该树下的项集
        vector<listNode> headlists = GetHeadList(frequentitems);             // 获取头表
        vector<SItem> datesets1 = Get_FPfile(frequentitems, headlists); // 再次删除不频繁的元素
        //for(int j=0;j<datesets1.size();j++)
        //    datesets1[j].push_back(headlist[i].data);
        FPNode *head = CreateFPtree(datesets1, headlists);                    // 以当前datesets重新建树
        // base.push_back(headlist[i].data);
        Getresult(headlists, head, item.items); // 再次得到结果
    }
 //   if(true);
}
void print()
{
    cout << "频繁项集共有：" << result.size() << "项" << endl;
    for (int i = 0; i < result.size(); i++)
    {
        for (int j = 0; j < result[i].items.size(); j++)
        {
            cout << result[i].items[j] << " ";
        }
        cout << "(" << result[i].count << ")" << endl;
    }
}
void print(vector<listNode>& headlist)

{
    for(int i=headlist.size()-1;i>=0;i--)
    {
        FPNode * p=headlist[i].next;
        while(p)
        {
            cout<<p->data<<" "<<p->count<<"    ";
            p=p->next;
        }
        cout<<endl;
    }
}
int main()
{

    // 读取数据集
    string filename = "/Users/wanglinhao/Desktop/retail.txt";
    vector<vector<int> > dateset = ReadDateSetFromFile(filename); // 读取数据
    size_d = dateset.size();
    vector<listNode> headlist = GetHeadList(dateset); // 获取头表
    // cout<<1<<endl;  //在这打印可以，在删表函数之后不可以，继续在GetFPfile中找错
    vector<vector<int> > datesets = Get_FPfile(dateset, headlist); // 删除之后的表
    
   /*
    for(int i=0;i<datesets.size();i++)
    {
        for(int j=0;j<datesets[i].size();j++)
        {
            cout<<datesets[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"__________________"<<endl;
    */
    // cout<<1<<endl;//上边问题解决
    // cout<<1<<endl;
    FPNode *head = CreateFPtree(datesets, headlist); // 建树
    // cout<<2<<endl;
    //print(headlist);
    // cout<<1<<endl;//建树没啥问题，但是时间有点长。。
    vector<int> base;
    Getresult(headlist, head, base);
    // cout<<3<<endl;
    print();
    return 0;
}