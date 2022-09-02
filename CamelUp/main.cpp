//
//  main.cpp
//  CamelUp
//
//  Created by andrew on 2022/8/23.
//

#include <iostream>
#include <vector>

void gameboardSetting(int& BoardLength,int& PlayerNumber);
double calculationProbablity();
double expectedCost();
std::string strategyResult();

//assumption: not many horses

class gameObject{
    public:
    int positionOnBoard;
};
class Player;
class Horse;
class Trap;
class Board{
    public:
    int BoardLength;
    std::vector<std::vector<Horse*>> position;
    std::vector<std::vector<Trap*>> trapPosition;
    Board(){
        BoardLength = 16;
        position = std::vector<std::vector<Horse*>>(16+3);
        trapPosition = std::vector<std::vector<Trap*>>(16+3);
    }
    Board(int l){
        BoardLength = l;
        position = std::vector<std::vector<Horse*>>(l+3);
        trapPosition = std::vector<std::vector<Trap*>>(l+1);
    }
};
class Horse:public gameObject{
    private:
//    std::vector<Player*> firstHorseGuess;
//    std::vector<Player*> lastHorseGuess;
    //has position
    public:
    int id = 0;
    int rank = 0;
    std::vector<Player*> firstHorseGuess;
    std::vector<Player*> lastHorseGuess;
    std::vector<Player*> legWinnerGuess;
    Horse(){
        positionOnBoard = 0;
//        std::cout<<"Horse Created"<<std::endl;
    }
    bool chooseFirst(Player* player,std::vector<Horse*> HorseList){
        for(auto p: firstHorseGuess){
            if(p == player)
                return false;
        }
        firstHorseGuess.push_back(player);
        return true;
    }
    bool chooseLast(Player* player,std::vector<Horse*> HorseList){
        for(auto p: lastHorseGuess){
            if(p == player)
                return false;
        }
        lastHorseGuess.push_back(player);
        return true;
    }
//    bool operator==(const Horse& h1) const;
    
    ~Horse(){
//        std::cout<<"Horse Deleted"<<std::endl;
    }
};
//bool Horse::operator==(const Horse& h1) const {
//    return x == rhs.x and y == rhs.y;
//}
class Trap: public gameObject{
    public:
    bool positive;
    Player* player = nullptr;
    Trap(){
        positive = true;
    }
    ~Trap(){
        std::cout<<"trap delete"<<std::endl;
    }
};
class Player{
    private:
    
    Player* partner = nullptr;
    public:
    bool trapUsed;
    int CurrentMoney;
    int sequence;
    Player(){
        trapUsed = false;
        CurrentMoney = 0;
    }
    
};
void moveHorse(Horse* currentHorse,Board* board,int moveIndex,bool isNegativeTrap){
    std::vector<Horse*> *start = &(board->position[currentHorse->positionOnBoard]);
    std::vector<Horse*> *destination = &(board->position[currentHorse->positionOnBoard + moveIndex]);
    if(currentHorse->positionOnBoard == 0){
        start->erase(find(start->begin(),start->end(),currentHorse));
        destination->push_back(currentHorse);
        currentHorse->positionOnBoard += moveIndex;
        if(board->trapPosition[currentHorse->positionOnBoard].empty() ==false){
            board->trapPosition[currentHorse->positionOnBoard][0]->player->CurrentMoney+=1;
            if(board->trapPosition[currentHorse->positionOnBoard][0]->positive == true)
                moveHorse(currentHorse, board, 1, false);
            else
                moveHorse(currentHorse, board, -1, true);
        }
        return;
    }
    if(currentHorse->positionOnBoard == 1 && isNegativeTrap == true){
        start->erase(find(start->begin(),start->end(),currentHorse));
        destination->push_back(currentHorse);
        currentHorse->positionOnBoard += moveIndex;
    }
        
    std::vector<Horse*> AllHorseMove;
    AllHorseMove.insert(AllHorseMove.end(),find(start->begin(),start->end(),currentHorse), start->end());
    start->erase(find(start->begin(),start->end(),currentHorse),start->end());
    for(auto h:AllHorseMove){
        h->positionOnBoard += moveIndex;
    }
    //check trap on board haven't been added
    if(isNegativeTrap == false)
        destination->insert(destination->end(),AllHorseMove.begin(),AllHorseMove.end());
    else
        destination->insert(destination->begin(),AllHorseMove.begin(),AllHorseMove.end());
    
    if(board->trapPosition[currentHorse->positionOnBoard].empty() ==false){
        board->trapPosition[currentHorse->positionOnBoard][0]->player->CurrentMoney+=1;
        if(board->trapPosition[currentHorse->positionOnBoard][0]->positive == true)
            moveHorse(currentHorse, board, 1, false);
        else
            moveHorse(currentHorse, board, -1, true);
    }
}
//check whether game has ended
bool checkEnd(std::vector<Horse*> HorseList){
    for(auto a: HorseList){
        if(a->positionOnBoard >= 16)
            return false;
    }
    return true;
}
void dice(std::vector<Horse*>& tempHorseList,Board* board){
    srand(time(0));
    int randHorse = rand() % tempHorseList.size();
    int randNumer = (rand() % 3) + 1;
//    tempHorseList[randHorse] -> positionOnBoard += randNumer;
    moveHorse(tempHorseList[randHorse], board, randNumer, false);
    tempHorseList.erase(tempHorseList.begin() + randHorse);
}
//print current Board
//can change to updateBoard later, so choice like be on horse doesn't change board
//If board size changed, this must change!
//need to implement rank
void printCurrentBoard(Board* board,std::vector<Horse*>& HorseList){
    //review string replace;
    bool startCount = false;
    int index = 0;
    std::cout<< "|||||||||" << std::endl;
    int i=0;
    for(auto p: board->position){
        if(p.empty()!=true && startCount==false){
            startCount = true;
            std::cout<< i << " block:";
        }
        if(startCount && index < HorseList.size()){
            std::cout<< "|" ;
            for(auto h:p){
                std::cout<<h->id;
                index++;
            }
            std::cout<< "|"<<std::endl;
        }
        i++;
    }
    std::cout<< "|||||||||" << std::endl;
    for(auto p: board->trapPosition){
        if(p.empty() == false)
            std::cout<<"Trap: "<<p[0]->positionOnBoard<<std::endl;
    }
}
//each player has decision
void decision(Player* currentPlayer,std::vector<Horse*>& HorseList,std::vector<Horse*>& tempHorseList,Board* board){
    int choice;
    std::cout<<"Your decision:"<<std::endl;
    std::cin >> choice;
    //type check!
    switch(choice){
        case 1:
            std::cout<<"Enter your horse numer"<<std::endl;
            int horseNumber;
            std::cin >> horseNumber;
            if(HorseList[horseNumber]->legWinnerGuess.size()>=5 || horseNumber < 0 || horseNumber >= HorseList.size()){
                std::cout<<"You cannot select this horse!"<<std::endl;
                decision(currentPlayer,HorseList,tempHorseList,board);
            }
            else
                HorseList[horseNumber]->legWinnerGuess.push_back(currentPlayer);
            break;
        case 2:
            std::cout<<"Enter your first horse numer"<<std::endl;
            int FirsthorseNumber;
            std::cin >> FirsthorseNumber;
            if(FirsthorseNumber < 0 || FirsthorseNumber >= HorseList.size() || !HorseList[FirsthorseNumber]->chooseFirst(currentPlayer, HorseList)){
                std::cout<<"You cannot select this horse!"<<std::endl;
                decision(currentPlayer,HorseList,tempHorseList,board);
            }
            break;
        case 3:
            std::cout<<"Enter your last horse numer"<<std::endl;
            int LasthorseNumber;
            std::cin >> LasthorseNumber;
            if(LasthorseNumber < 0 || LasthorseNumber >= HorseList.size() || !HorseList[LasthorseNumber]->chooseLast(currentPlayer, HorseList)){
                std::cout<<"You cannot select this horse!"<<std::endl;
                decision(currentPlayer,HorseList,tempHorseList,board);
            }
            break;
        case 4:
            if(currentPlayer->trapUsed == true)
                std::cout<<"Trap has been used"<<std::endl;
            else{
                int inputposition;
                int num;
                std::cout<<"Trap position"<<std::endl;
                std::cin >> inputposition;
                
                if(inputposition <= 0 || inputposition > board->trapPosition.size() || (inputposition > 0 && board->trapPosition[inputposition-1].empty() == false) ||  (inputposition < board->trapPosition.size()-1 && board->trapPosition[inputposition+1].empty() == false) || board->position[inputposition].empty() == false || board->trapPosition[inputposition].empty() == false){
                    std::cout<<"Position cannot be used for trap"<<std::endl;
                    std::cout<< ((inputposition > 0 && board->trapPosition[inputposition-1].empty() == false));
                    std::cout<<(inputposition < board->trapPosition.size()-1 && board->trapPosition[inputposition+1].empty() == false);
                    std::cout<<(board->position[inputposition].empty() == false);
                    std::cout<<(board->trapPosition[inputposition].empty() == false);
                    std::cout<<board->trapPosition.size();
                    
                    decision(currentPlayer,HorseList,tempHorseList,board);
                }
                else{
                    Trap* t = new Trap();
                    t->player = currentPlayer;
                    t->positionOnBoard = inputposition;
                    std::cout<<">=0 num means it's positive, "<<std::endl;
                    std::cin >> num;
                    currentPlayer->trapUsed = true;
                    if(num<0)
                        t->positive = false;
                    else
                        t->positive = true;
                    board->trapPosition[inputposition].push_back(t);
                }
            }
            break;
        default:
            dice(tempHorseList,board);
            currentPlayer->CurrentMoney += 1;
            printCurrentBoard(board,HorseList);
    }
}
//for each round, we go through each player decision,if one round end, player doesn't start from 0
void round(std::vector<Player*>& PlayerList,std::vector<Horse*>& HorseList,std::vector<Horse*>& tempHorseList,int& currentPlayer,Board* board){
    while(!(tempHorseList.size() == 0 || checkEnd(HorseList) == false)){
        decision(PlayerList[currentPlayer % PlayerList.size()],HorseList,tempHorseList,board);
        currentPlayer++;
    }
    for(auto h:HorseList){
        h->legWinnerGuess.clear();
    }
    for(auto p:board->trapPosition){
        if(p.empty() == false){
            if(p[0] != nullptr)
                delete p[0];
            p.clear();
        }
    }
    for(auto p:PlayerList)
        p->trapUsed = false;
//    board->trapPosition.clear();
    for(auto p:board->trapPosition){
        std::cout<<p.size()<<std::endl;
    }
}
void sortRank(Board* b,int horseNum){
    //rank horsenum to 1
    for(auto p:b->position){
        for(auto h:p){
            h->rank = horseNum;
            horseNum--;
        }
    }
}
//haven't consider rank though
bool compareHorse(Horse* h1, Horse* h2)
{
    return (h1->rank < h2->rank);
}

//reward player after a round 1st get 5 and 2nd get 2 other get -1
void rewardPlayerRound(std::vector<Player*> PlayerList,std::vector<Horse*> HorseList,Board* board){
    sortRank(board, HorseList.size());
    std::vector<Horse*> rankList = HorseList;
    sort(rankList.begin(),rankList.end(),compareHorse);
    int first[5] = {5,3,2,2,2};
    int second[5] = {2,2,2,2,2};
    int others[5] = {-1,-1,-1,-1,-1};
    for(int j=0;j<rankList[0]->legWinnerGuess.size();j++){
        rankList[0]->legWinnerGuess[j]->CurrentMoney += first[j];
    }
    for(int j=0;j<rankList[1]->legWinnerGuess.size();j++){
        rankList[1]->legWinnerGuess[j]->CurrentMoney += second[j];
    }
    for(int i=2;i<rankList.size();i++){
        for(int j=0;j<rankList[i]->legWinnerGuess.size();j++){
            rankList[i]->legWinnerGuess[j]->CurrentMoney += others[j];
        }
    }
    for(auto p: PlayerList){
        std::cout<<"Player money is:";
        std::cout<<p->CurrentMoney<<std::endl;
    }
}
//rewardPlayerAtLast
void rewardPlayerAtLast(std::vector<Player*> PlayerList,std::vector<Horse*> HorseList,Board* board){
    sortRank(board, HorseList.size());
    std::vector<Horse*> rankList = HorseList;
    sort(rankList.begin(),rankList.end(),compareHorse);
    //fix player for now
    const int size = 8;
    int first[size] = {8,5,3,2,2,1};
    int last[size] = {8,5,3,2,2,1};
    for(int i=6;i<PlayerList.size();i++){
        first[i] = -1;
        last[i] = -1;
    }
    //need modify if they are private!
    for(int j=0;j<rankList.size();j++){
        if(j==0){
            for(int i=0;i<rankList[j]->firstHorseGuess.size();i++){
                rankList[j]->firstHorseGuess[i]->CurrentMoney+=first[i];
            }
            for(auto p:rankList[j]->lastHorseGuess)
                p->CurrentMoney -= 1;
        }
        else if(j==rankList.size()-1){
            for(int i=0; i<rankList[j]->lastHorseGuess.size();i++){
                rankList[j]->lastHorseGuess[i]->CurrentMoney+=last[i];
            }
            for(auto p:rankList[j]->lastHorseGuess)
                p->CurrentMoney -= 1;
        }
        else{
            for(auto p:rankList[j]->firstHorseGuess)
                p->CurrentMoney -= 1;
            for(auto p:rankList[j]->lastHorseGuess)
                p->CurrentMoney -= 1;
        }
    }
    for(auto p: PlayerList){
        std::cout<<"Player money is:";
        std::cout<<p->CurrentMoney<<std::endl;
    }
}
//start game
void gameboardStart(){
    //initilization
    int BoardLength = 16;
    int PlayerNumber = 5;
    int HorseNumber = 5;
    std::vector<std::vector<int>> BoardVector = std::vector<std::vector<int>>(BoardLength);
    std::vector<Player*> PlayerList = std::vector<Player*>(PlayerNumber);
    std::vector<Horse*> HorseList = std::vector<Horse*>(HorseNumber);
    Board* board = new Board(BoardLength);
    
    for(int i=0;i<PlayerNumber;i++){
        Player* p = new Player();
        p->sequence = i;
        PlayerList[i] = p;
    }
    for(int i=0;i<HorseNumber;i++){
        Horse* h = new Horse();
        HorseList[i] = h;
        h->id = i;
    }
    for(auto h:HorseList)
        board->position[0].push_back(h);
    //strart game, count each round
    int roundCheck = 500;
    int currentPlayer = 0;
    while(checkEnd(HorseList) && roundCheck > 0){
        std::vector<Horse*> tempHorseList = HorseList;
        round(PlayerList,HorseList,tempHorseList,currentPlayer,board);
        rewardPlayerRound(PlayerList,HorseList,board);
        roundCheck--;
    }
    //check find reward
    rewardPlayerAtLast(PlayerList,HorseList,board);
    //delet objects in heap
    for(int i=0;i<PlayerNumber;i++){
        delete PlayerList[i];
    }
    for(int i=0;i<HorseNumber;i++){
        delete HorseList[i];
    }
    delete board;
}

int main(int argc, const char * argv[]) {
    gameboardStart();
    return 0;
}

