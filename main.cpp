/***************************************
    热带岛屿Lagrishan的首领现在面临一个问题：
    几年前，一批外援资金被用于维护村落之间的道路，
    但日益繁茂的丛林无情的侵蚀着村民的道路，导致道路维
    修开销巨大，长老会不得不放弃部分道路的维护。上图
    左侧图显示的是正在使用道路的简图以及每条路每个月的
    维修费用（单位为aacms）。现在长老会需要提出一种方案，
    即需要保证村落之间都可以互相到达，又要将每个月的道路
    维修费用控制在最小。村子编号为从A到I。上图右侧显示
    的方案最小维修开销为216 aacms每月。

    输入
    输入包含1~100个数据集，最后一行为0.每个数
    据集第一行为村落数目n, 1 < n < 27,依次用字母
    表的前n个字母标记。接下来有n-1行，每行的第一
    个数据便是按字母顺序排列的村子编号（不包括最
    后一个村庄）。每个村庄后面的数据k代表该村庄通
    往编号在其之后的村庄的道路数目，如A 2 B 12 I 25，
    代表A村庄有2个编号在A之后的村庄和其相连。若k大
    于0，k后面会依次给出这k个村庄的编号以及各自到
    起始村庄的道路维修费用，如A 2 B 12 I 25，代表A和
    B之间道路维修费用为12， A和I之间道路维修费用为25
    （维修费用为不超过100的正整数）.路的总数目不超过
    75条，每个村庄到其他村庄不会有超过15条路（包括编
    号在其之前和之后的）。
    输出
    每个数据集有一个输出：针对解决方案每个月维修道路的小费用。
    提示：蛮力算法虽能找出解决方案，但将会超出时间限制。
    样例输入
    9
    A 2 B 12 I 25
    B 3 C 10 H 40 I 8
    C 2 D 18 G 55
    D 1 E 44
    E 2 F 60 G 38
    F 0
    G 1 H 35
    H 1 I 35
    3
    A 2 B 10 C 40
    B 1 C 20
    0
    样例输出
    216
    30
*****************************************************/

#include <map>
#include <iostream>
#include <string>

struct Node {
    char NodeName;                         // 节点的名字
    std::map<Node *, int> allConnectNodes; // 联通的节点(节点，到该节点的距离)
    bool operator <(Node *other)           // 增加比较函数，使可以使用map
    {
        return this->NodeName < other->NodeName;
    }
};

Node *findNode(Node *start, char name, Node *except, Node **max, int *maxLen)
{
    auto node = start->allConnectNodes.begin();
    while (node != start->allConnectNodes.end())
    {
        Node *tmp = node->first;
        int len = node->second;
        if (except != tmp)
        {
            if (tmp->NodeName == name)
            {
                if (max != nullptr && maxLen != nullptr)
                {
                    if (*maxLen < len)
                    {
                        max[0] = tmp;
                        max[1] = start;
                        *maxLen = len;
                    }
                }
                return tmp;
            }
            else if (auto find = findNode(tmp, name, start, max, maxLen))
            {
                if (max != nullptr && maxLen != nullptr)
                {
                    if (*maxLen < len)
                    {
                        max[0] = tmp;
                        max[1] = start;
                        *maxLen = len;
                    }
                }
                return find;
            }
        }

        ++node;
    }

    return nullptr;
}

Node *createNode(Node *parent, char name, int len)
{
    Node *node = new Node;
    node->NodeName = name;
    if (parent != nullptr)
    {
        node->allConnectNodes[parent] = len;
    }
    return node;
}

void addNode(Node *root, char name, int len)
{
    Node *max[2];
    int maxLen = len;

    if (auto node = findNode(root, name, nullptr, max, &maxLen))
    {
        // 找到节点（说明构成了环状），并且可以拿到到该环中最长的一段
        if (maxLen != len)
        {
            // 从最长的节点剪断环
            max[0]->allConnectNodes.erase(max[0]->allConnectNodes.find(max[1]));
            max[1]->allConnectNodes.erase(max[1]->allConnectNodes.find(max[0]));

            // 把当前的节点加入进去
            root->allConnectNodes[node] = len;
            node->allConnectNodes[root] = len;
        }
    }
    else
    {
        auto newNode = createNode(root, name, len);
        root->allConnectNodes[newNode] = len;
    }
}

int getAllLen(Node *root, Node *except)
{
    int len = 0;
    if (root == nullptr)
    {
        return len;
    }

    auto node = root->allConnectNodes.begin();
    while (node != root->allConnectNodes.end())
    {
        auto tmp = node->first;
        if (tmp != except)
        {
            len += node->second;
            len += getAllLen(tmp, root);
        }

        ++node;
    }
    return len;
}

int getNodeCount(Node *root, Node *except)
{
    int count = 0;
    if (root == nullptr)
    {
        return count;
    }

    auto node = root->allConnectNodes.begin();
    while (node != root->allConnectNodes.end())
    {
        auto tmp = node->first;
        if (tmp != except)
        {
            count++;
            count += getNodeCount(tmp, root);
        }
        ++node;
    }

    return count;
}

void parse(Node **root, const char *line)
{
    // 这里没判断传进来的字符串是不是有效的，调用者自己保证
    const char *split = " ";
    char *copy = strdup(line);
    char *all[27 * 2] = {0};
    char **p = all;
    char *str = strtok(copy, split);
    while( str != nullptr )
    {
        *p = str;
        ++p;

        str = strtok(nullptr,split);
    }

    Node *parent = nullptr;
    if (*root == nullptr)
    {
        *root = createNode(nullptr, all[0][0], 0);
        parent = *root;
    }
    else
    {
        parent = findNode(*root, all[0][0], nullptr, nullptr, nullptr);
    }

    // 确保找到父节点
    if (parent != nullptr)
    {
        for (int i = 2; all[i] != nullptr; i+=2)
        {
            char name = all[i][0];     // 确保是字母
            int len = atoi(all[i+1]);  // 确保是数字
            addNode(parent, name, len);
        }
    }

    free(copy);
}

int main(int, char *[])
{
    Node *root = nullptr;
    parse(&root, "A 2 B 12 I 25");
    parse(&root, "B 3 C 10 H 40 I 8");
    parse(&root, "C 2 D 18 G 55");
    parse(&root, "D 1 E 44");
    parse(&root, "E 2 F 60 G 38");
    parse(&root, "F 0");
    parse(&root, "G 1 H 35");
    parse(&root, "H 1 I 35");

    std::cout << "min len: "
              << getAllLen(root, nullptr) << std::endl;
    std::cout << "node count: "
              << getNodeCount(root, nullptr) + 1 << std::endl;
    return 0;
}
