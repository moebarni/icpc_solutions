#include <iostream>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <set>
#include <string.h>

using namespace std;

//possible directions for the laser beam
#define N 0
#define E 1
#define S 2
#define W 3
#define MAX 2000000

int r, c, tree[MAX], lowestX = 1000000000, lowestY = 1000000000;
bool without;
typedef pair<int,int> p;
typedef long long int big;

set<p> rows[MAX];
set<p> columns[MAX];

//line objects for horizontal and vertical laser sections
class line{
    public:
        //start and end points of straight lines
        int xlow, ylow, xhigh, yhigh;
    
        line(int xl, int yl, int xh, int yh) : xlow(xl), ylow(yl), xhigh(xh), yhigh(yh){} 
};

//events for plane-sweep
class event{
    public:
        bool vertical;
        int value, low, high, add;

        event(bool ver, int val, int l, int h, int a) : vertical(ver), value(val), low(l), high(h), add(a){}

        bool operator < (const event &other)const{
            if(value != other.value){
                return value < other.value;
            }else if(add != other.add){
                return add > other.add;
            }else{
                return vertical > other.vertical;
            }
        }
};

//simulation of a laser beam as lines
void laserbeam(vector<line> &hori, vector<line> &verti, int x, int y, int dir){ 
    int nextX, nextY;
    bool next = true;

    while(next){
        //West
        if(dir == W){
            nextX = x;   
            nextY = 0;   //default
            auto it = rows[x].lower_bound({y,-1});

            if(it == rows[x].begin()){
                next = false;
            }else{
                it--;
                nextY = it->first;
                //next direction
                if(it->second){
                    dir = N;
                }else{
                    dir = S;
                }
            }
        //South 
        }else if(dir == S){
            nextX = r+1;   //default
            nextY = y;   
            auto it = columns[y].upper_bound({x,1000000000});

            if(it == columns[y].end()){
                next = false;
            }else{
                nextX = it->first;
                //next direction
                if(it->second){
                    dir = E;
                }else{
                    dir = W;
                }
            }
        //East           
        }else if(dir == E){
            nextX = x;   
            nextY = c+1;   //default
            auto it = rows[x].upper_bound({y,1000000000});

            if(it == rows[x].end()){
                next = false;
            }else{
                nextY = it->first;
                //next direction
                if(it->second){
                    dir = S;
                }else{
                    dir = N;
                }
            }
        //North
        }else if(dir == N){
            nextX = 0;   //default
            nextY = y;   
            auto it = columns[y].lower_bound({x,-1});

            if(it == columns[y].begin()){
                next = false;
            }else{
                it--;
                nextX = it->first;
                //next direction
                if(it->second){
                    dir = W;
                }else{
                    dir = E;
                }
            }            
        }

        if(nextX == x){
            hori.emplace_back(line{x, min(y, nextY)+1, x, max(y, nextY)-1});
        }else if(nextY == y){
            verti.emplace_back(line{min(x, nextX)+1, y, max(x, nextX)-1, y});
        }
        x = nextX;
        y = nextY;
    }
    //ends the laser in the detector
    if(x == r && y == c+1){
        without = true;
    }
}

//adds the value (= "add") to the given position and all subsequent positions (in the tree)
void update(int pos, int value){
    if(pos <= 0){
        return;
    }
    for(pos; pos < MAX; pos += (pos & -pos)){
        tree[pos] += value;
    }
}

//computes the prefix sum for a given position (from the tree)
big prefixSum(int pos){
    big result = 0;
    if(pos <= 0){
        return 0;
    }
    for(pos; pos>0; pos -= (pos & -pos)){
        result += tree[pos];
    }
    return result;
}

//computes the number of horizontal lines which cross the given section
big intervalSum(int left, int right){
    if(left>right){
        return 0;
    }
    return prefixSum(right) - prefixSum(left-1);
}

//computes all intersections for horizontal and vertical lines and stores the lexicographically smallest
big intersections(vector<line> hori, vector<line> verti){
    memset(tree,0,sizeof tree);
    vector<event> e;
    big result = 0;

    //generate all events
    for(auto h : hori){
        e.emplace_back(false, h.ylow, h.xlow, h.xlow, 1);
        e.emplace_back(false, h.yhigh, h.xlow, h.xlow, -1);
    }
    for(auto v : verti){
        e.emplace_back(true, v.ylow, v.xlow, v.xhigh, -1);
    }
    sort(e.begin(), e.end());

    for(auto e1 : e){
        //vertical line
        if(e1.vertical){
            int low = e1.low;
            int high = e1.high;
            big inter = intervalSum(low, high);
            if(inter <= 0){
                continue;
            }
            //binary search to find precise intersection
            while(low < high){
                int mid = (low + high)/2;
                if(intervalSum(e1.low, mid) >= 1){
                    high = mid;
                }else{
                    low = mid+1;
                }
            }
            //keep track of the lexicographically smallest mirror
            if(low < lowestX){
                lowestX = low;
                lowestY = e1.value;
            }else if(low == lowestX && e1.value < lowestY){
                lowestY = e1.value;
            }            
            result += inter;

        //horizontal line
        }else{
            update(e1.low, e1.add);
        }
    }
    return result;
}

int main()
{   
    int curCase = 0;
    int m, n, x, y;

    while(scanf("%d%d%d%d", &r, &c, &m, &n) > 0){
        curCase++;

        //empty the sets rows and columns
        for(int i=0; i<=r+1; i++){
            rows[i].clear();
        }
        for(int i=0; i<=c+1; i++) {
            columns[i].clear();
        }

        //fill the rows and colums sets with "/"-mirrors (represented by 0)
        for(int i=0; i<m; i++){
            scanf("%d%d", &x, &y);
            rows[x].insert({y, 0});
            columns[y].insert({x, 0});
        }

        //fill the rows and colums sets with "\"-mirrors (represented by 1)
        for(int i=0; i<n; i++){
            scanf("%d%d", &x, &y);
            rows[x].insert({y, 1});
            columns[y].insert({x, 1});
        }

        //resets
        lowestX = 1000000, lowestY = 1000000;
        without = false;

        //laser starting at the top left of the grid (start point)
        vector<line> horiStart;
        vector<line> vertiStart;
        laserbeam(horiStart, vertiStart, 1, 0, E);

        //safe opens without inserting a mirror
        if(without){
            cout << "Case " << curCase << ": 0\n";
            continue;
        }

        //laser starting at the bottom right of the grid (end point)
        vector<line> horiEnd;
        vector<line> vertiEnd;
        laserbeam(horiEnd, vertiEnd, r, c+1, W);

        //count intersections
        big num = 0;
        num += intersections(horiStart, vertiEnd);
        num += intersections(horiEnd, vertiStart);

        //output
        if(num == 0){
            cout << "Case " << curCase << ": impossible\n";
        }else{
            cout << "Case " << curCase << ": " << num << " " << lowestX << " " << lowestY << '\n';
        }
    }
    return 0;
}