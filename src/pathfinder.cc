#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <queue>
#include <set>
#include <tuple>

typedef std::tuple<int,int> position;



void print_dist_map(const int dist_map[],const unsigned char map[], int x2, int y2, const int w, const int h) {
    printf("----- DIST MAP -----");
    int end = y2*w+x2;
    for(int y=0;y<h;y++) {
        printf("\n");
        for(int x=0;x<w;x++) {
            int ind = y*w+x;
            int dist = dist_map[ind];
            if (ind == end)
                printf("FF ");
            else if (dist != -1)
                printf("%02d ",dist);
            else if (map[ind]==0)
                printf("|| ");
            else
                printf("   ");
            
        }
    }
    printf("\n");
}


int FindPath(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY, const unsigned char map[], const int nMapWidth, const int nMapHeight, int *pOutputBuffer, const int nOutputBufferSize) {
    int * pDistMap = new int[nMapWidth*nMapHeight];
    int nDistance = -1;
    
    // Set all positions in distance map to -1 (unvisited)
    memset(pDistMap,-1,nMapWidth*nMapHeight*sizeof(int));
    
    // init result array
    memset(pOutputBuffer,0,sizeof(int) * nOutputBufferSize);
    
    // Compare manhattan dist to target plus number of steps from starting position
    auto cmp = [nTargetX,nTargetY,pDistMap,nMapWidth] (const position& left, const position& right) -> bool {
        int left_x,left_y,right_x,right_y;
        std::tie(left_x,left_y) = left;
        std::tie(right_x,right_y) = right;
        
        // Calculate manhattan distance to target
        int left_dist = abs(nTargetX-left_x) + abs(nTargetY-left_y);
        int right_dist = abs(nTargetX-right_x) + abs(nTargetY - right_y);
        
        // Get distance traveled so far
        int left_curr_dist = pDistMap[left_y*nMapWidth+left_x];
        int right_curr_dist = pDistMap[right_y * nMapWidth + right_x];
        
        // Score is travaled distance plus distance to target
        int left_score = left_dist + left_curr_dist;
        int right_score = right_dist + right_curr_dist;
        return left_score < right_score;
    };
    
    // List of positions to be evaluated next
    std::multiset<position,decltype(cmp)> nextPositionSet(cmp);
    
    // Init first position and dist map
    nextPositionSet.insert( std::make_tuple(nStartX,nStartY));
    pDistMap[nStartY * nMapWidth + nStartX] = 0; // distance zero at starting position
    
    while( !nextPositionSet.empty() ) {
        int nX,nY;
        auto& pos = *(nextPositionSet.begin());
        std::tie(nX,nY) = pos;
        
        // Is current position the destination?
        if ( nX == nTargetX && nY == nTargetY ) {
            nDistance = pDistMap[nTargetY*nMapWidth+nX];
            break;
        }
        
        nextPositionSet.erase( nextPositionSet.begin());
        
        // Calculate next dist
        int nNextDist = pDistMap[nY*nMapWidth + nX] + 1;
        
        // Create positions to adjacent nodes
        std::queue<position>  nextStepsQueue;
        nextStepsQueue.push( std::make_tuple(nX-1,nY) );
        nextStepsQueue.push( std::make_tuple(nX,nY-1) );
        nextStepsQueue.push( std::make_tuple(nX+1,nY) );
        nextStepsQueue.push( std::make_tuple(nX,nY+1) );
        
        // Iterate through list of next steps
        while( !nextStepsQueue.empty() ) {
            int nNextX,nNextY;
            std::tie(nNextX,nNextY) = nextStepsQueue.front();
            int nNextIndex = nNextY * nMapWidth + nNextX;
            
            // Is next position inside bounds and a passable position?
            if (nNextX>=0 && nNextX<nMapWidth && nNextY>=0 && nNextY<nMapHeight && map[ nNextIndex ] == 1) {
                // Fetch distance at next step if any
                int nCurrentDistAtNextStep = pDistMap[nNextIndex];
                
                // If distance is unvisited or greater than current, set new distance
                if ( nCurrentDistAtNextStep == -1 || nCurrentDistAtNextStep > nNextDist) {
                    auto& step = nextStepsQueue.front();
                    pDistMap[nNextIndex] = nNextDist;
                    
                    // This position should be evaluated next..
                    nextPositionSet.insert(step);
                    
                    // Print distance map
                    print_dist_map(pDistMap,map,nTargetX,nTargetY,nMapWidth,nMapHeight);
                }
            }
            // pop item
            nextStepsQueue.pop();
        }
    }
    
    if (nDistance != -1) {
        // Backtrack from final position, next hop is where distance is -1
        //printf("OK Dist = %d\n",pDistMap[nTargetY*nMapWidth+nTargetX]);
        
        int x=nTargetX, y=nTargetY;
        int prev_dist = nDistance-1;
        while(!(x == nStartX && y == nStartY)) {
            // Build path
            pOutputBuffer[prev_dist] = y*nMapWidth + x;
            
            //printf("x=%d, y=%d, index=%d, dist=%d\n",x,y,y*w+x,prev_disti);
            
            // Find direction where distance is decremented
            if (x>0 && pDistMap[y*nMapWidth+x-1] == prev_dist) {
                x -=1;
            } else if ((x+1)<nMapWidth && pDistMap[y*nMapWidth+x+1] == prev_dist) {
                x +=1;
            } else if (y>0 && pDistMap[(y-1)*nMapWidth+x] == prev_dist) {
                y -=1;
            } else if ((y+1)<nMapHeight && pDistMap[(y+1)*nMapWidth+x] == prev_dist) {
                y +=1;
            }
            prev_dist--;
        }
        
    } else {
        printf("Not solvable!!!\n");
    }
    
    delete[] pDistMap;
    return nDistance;
}



void test_path1() {
    unsigned char map[] = {1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1};
    int res[100];
    int x1=0,y1=0,x2=1,y2=2,w=4,h=3;
    int distance=FindPath(x1,y1,x2,y2,map,w,h,res,100);
    if (distance > 0 ) {
        for(int i =0;i<distance;i++) {
            printf("%d,",res[i]);
        }
        printf("\n");
    }
}

void test_path2() {
    unsigned char map[] = {1, 0, 1, 1,  1,0,1,0 , 1,1,0,1, 1,0,0,1 };
    int res[100];
    int x1=2,y1=0,x2=3,y2=2,w=4,h=4;
    int distance=FindPath(x1,y1,x2,y2,map,w,h,res,100);
    if (distance > 0 ) {
        for(int i =0;i<distance;i++) {
            printf("%d,",res[i]);
        }
        printf("\n");
    }
}


void test_2020_path() {
    unsigned char map[] = {
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,
        1,1,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,
        1,1,0,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,
        1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,
    };
    
    int res[100];
    int x1=0,y1=19,x2=17,y2=6,w=20,h=20;
    int dist = FindPath(x1,y1,x2,y2,map,w,h,res,100);
    printf("Dist = %d\n",dist);
    for(int i=0;i<dist;i++)
        if (i==0) printf("(%d,",res[i]);
        else if ( i==(dist-1)) printf("%d)\n",res[i]);
        else printf("%d,",res[i]);
    
}

int main(int argc, char *argv[]) {
    
    if (argc == 3 || argc == 4) {
        // get map
        int nWidth = atoi(argv[1]);
        int nHeight = atoi(argv[2]);
        int nSize = nWidth*nHeight;
        unsigned char * map = new unsigned char[nSize];
        int * res = new int[nSize];
        srand( argc == 3 ? (unsigned int)time(NULL) : (unsigned int)atol(argv[3]));
        int x1 = rand() % nWidth;
        int x2 = rand() % nWidth;
        int y1 = rand() % nHeight;
        int y2 = rand() % nHeight;
        for(int i=0;i<nSize;i++) {
            int num = rand() % 100;
            
            map[i] = num < 70 || i==(y1*nWidth+x1) || i==(y2*nWidth+x2) ? 1 : 0;  
        }
        
        int nDistance = FindPath(x1,y1,x2,y2,map,nWidth,nHeight,res,nSize);
        if (nDistance > 0 ) {
            for(int i =0;i<nDistance;i++) {
                printf("%d,",res[i]);
            }
            printf("\n");
        }
        printf("distance=%d\n",nDistance);
        delete[] map;
        delete[] res;
    } else {
        test_2020_path();
        test_path1();
    }
    
    return 0;
}
