// fruit_bot.c
// Assignment 3, COMP1511 18s1: Fruit Bot
//
// This program by Ali-Ahsan-Ali (z5207397) on 26-05-18
//
// Version 1.0.0: Assignment released.
// Version 1.0.1: minor bug in main fixed

// Single_bot_worlds works better when "bots & bots1" are not
// declared or initatilised, but work much better in multi_bot_worlds.
// so please explain to me y when u mark this.ty
// another thing to note is
//(profit/distancebetweenbuyerandseller)/distance to bot
//is the best formula, but it gets destroyed in multibot worlds as everyone uses
//the same formula

// Another note, where it says bot*5 + 10, it should say bot*quantity_to_be_bought
// but due to runtime, it had to be removed.

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "fruit_bot.h"
#include <stdlib.h>
#include <math.h>



#define FALSE       98
#define TRUE        99
#define LAST_BUY     3
#define EMPTY        0
#define AT_LOCATION  0
#define LAST_MOVE    1

void print_player_name(void);
void print_move(struct bot *b);
void run_unit_tests(void);

// ADD PROTOTYPES FOR YOUR FUNCTIONS HERE
int nearest_electricity(struct bot *b);
int distance_buy_sell (struct bot *b, struct location *opt);
int max_distance_to_charge(struct bot *b);
int nearest_compost(struct bot *b);
int optimal_profit_distance (struct bot *b);
int optimal_distance (int distance, int distance1, struct bot *b);
int fruit_buyer (struct bot *b);
int fruit_seller (struct bot *b);
int quantity_to_be_bought (struct bot *b);
int number_of_bots (struct location *curr_location);
int number_of_bots_single (struct bot *b);
int world_size(struct bot *b);

// YOU SHOULD NOT NEED TO CHANGE THIS MAIN FUNCTION

int main(int argc, char *argv[]) {
    
    if (argc > 1) {
        // supply any command-line argument to run unit tests
        run_unit_tests();
        return 0;
    }
    struct bot *me = fruit_bot_input(stdin);
    if (me == NULL) {
        print_player_name();
    } else {
        print_move(me);
    }
    return 0;
}

void print_player_name(void) {
    // CHANGE THIS PRINTF TO YOUR DESIRED PLAYER NAME
    printf("bilch");
}

void print_move(struct bot *b) {
    int electricity = nearest_electricity(b);
    int seller = fruit_seller (b);
    int buyer = fruit_buyer (b);
    int move = 0;
    int max_moves = b->turns_left;
    int max_move_this_turn = b->maximum_move;
    int cash = b->cash;
    int inventory = b-> fruit_kg;
    int optimal_quantity = quantity_to_be_bought (b);
    
    // if buyer == AT_LOCATION sell before moving to recharge
    //also means u wont have no money when recharging
    if (b->battery_level <= max_distance_to_charge(b)
        && electricity != FALSE && buyer != AT_LOCATION
        && (inventory == EMPTY || cash > b->battery_capacity)){
        if (electricity == 0){
            printf("Buy %d\n", b->battery_capacity);
        }else{
            printf("Move %d\n", electricity);
        }
    }else if (b->battery_level == 0){
        //peacefull death
        printf("Move 0\n");
    }else if (inventory == EMPTY && b->location->price == 0){
        //move to the seller over the course of turns and then buy
        // what the seller has to sell
        printf("Move %d\n", seller);
    }else if (inventory == EMPTY && seller != AT_LOCATION){
        printf("Move %d\n", seller);
    }else if (inventory == EMPTY && seller == AT_LOCATION){
        if (optimal_profit_distance (b) > b->battery_level){
            //dont buy if u cant sell before u run our of elec
            printf("Move 0\n");
        }else if (max_moves < LAST_BUY){
            //dont buy if round is about to end && impossible to make
            //profit off the buy
            printf("Move 0\n");
        }else if (max_moves == LAST_BUY &&
                  max_move_this_turn > optimal_profit_distance (b)){
            //dont buy if round is about to end
            //3 is a magic number as if you buy that means u have 2 rounds
            //to move and sell assuming the movement is within
            //max_moves_this_turn
            printf("Buy %d\n", optimal_quantity);
        }else{//buy ur stuff
            printf("Buy %d\n", optimal_quantity);
        }
    }else if (inventory > EMPTY && buyer != AT_LOCATION && buyer != FALSE){
        //move the the buyer of ur fruits and then sell them
        printf("Move %d\n", buyer);
    }else if (inventory > EMPTY && buyer == FALSE){
        //if you somehow (this really shouldn't happen) buy more than can be
        //sold anywhere, sell it to the compost
        if (nearest_compost(b) == AT_LOCATION){
            printf("Sell %d\n", inventory);
        }else{
            printf("Move %d\n", nearest_compost(b));
        }
    }else if (inventory > EMPTY && buyer == AT_LOCATION){
        //sell ur stuff when ur at the selling place
        printf("Sell %d\n", inventory);
    }else if (max_moves == LAST_MOVE){
        // peaceful death
        move = b->battery_level;
        printf("Move 0 \n");
    }else{
        //this should be an error if executed
        move = b->battery_level;
        printf("Move%d\n", move);
    }
}

// ADD A COMMENT HERE EXPLAINING YOUR OVERALL TESTING STRATEGY
// The Overall testing statergy was centred around testing each function
// to give the correct output, this was done through the use of debugging
// printf statement's and comparison between user output vs round output
void run_unit_tests(void) {
    // PUT YOUR UNIT TESTS HERE
    // This is a difficult assignment to write unit tests for,
    // but make sure you describe your testing strategy above.
    /*The first function tested was max_distance_to_chage, which returned half
     the world size, then added on maximum move of the bot. As these remain
     static, they were observed within all printf statements in various
     rounds to maintain the same integer. Also by 'hand counting' by me
     It could be seen as correct. The above also holds true for the world_size,
     especially in the case of non-multibot worlds.
     
     Fruit_seller was the next function to be tested using the same methods
     (printf statements). It was seen to enter the first selling place, then
     going through every single buying place for the same fruit. It always
     returns the highest profit place (unless it circles around due to supply
     and demand of fruit within multibot worlds, which could have been fixed
     by finding the demand through quantity_to_be_bought function which creates 
     a runtime error). This error is due to cpu limits and other bots having
     circling around the same place. But once given a proper, single bot input
     without accounting for the multibot world, it returned out the most optimal
     output.
     
     NOTE:There is also an issue with single bot (in both buyer and seller func)
     worlds where (due to bots being uninitialised and == 0) the bot circles
     around as the bots at the selling/buying location changes when you enter
     said location.Furthermore, 
     profit/distance between buyer and seller)/distance
     is a superior strategy in single bot simulations, but in multibot
     simulations, it is better to leave the pack for your own individual
     profit, thus has been removed from the program all together. I should have
     returned a struct and used all that useful info rather than multiple
     functions of the exact thing. 
     
     Since the fruit_seller, optimal_distance, quantity_to_buy functions are all
     copies of the function noted above, the same holds true. Differences where
     yet again tested by printf statements.
     
     nearest_electricityhad been tested through the lab autotest program, also
     numerical hand counting from the output and printf statements
     confirm the functions output.
     
     distance_buy_sell is a copy of the nearest_electricity function, only
     changing strcmp(fruit, "Electricity") == 0 to
     strcmp(fruit, curr->name) == 0, as you go through the locations. This was
     also tested in the same way as the electricity function, through printf
     statements and by a small extension, the electricity autotest.
     
     nearest_compost again is a copy of the nearest_electricity function the
     difference again being in the strcmp (using "Anything" instead of
     "Electricity").Also had the same testing methods as mentioned above.
     
     number_of_bots at a location stayed static within wingle bot simulations
     (either 0 or 1 as there was me and nothing else) and was easy to test in
     single bot worlds. This was difficult to test in multibot worlds, but
     addition of all bots in the world and comparison with the leaderboard
     showed the value returned to be true.
     
     int number_of_bots_single the printf value compared with the leaderboard
     was always true, the easiest function to test.
     
     This concludes my testing, which solely included printf tests as all my
     functions used struct bot *b as an input.
     
     */
}


// ADD YOUR FUNCTIONS HERE

//Returns the threshold, below which, the bot needs to chaarge immediately
int max_distance_to_charge(struct bot *b){
    int i = 0;
    struct location *curr_location = b->location;
    while (curr_location != b->location || i == 0){
        curr_location = curr_location->east;
        i++;
    }
    i = i/2; // as you can reach a point by going west and east,
    //therefore u dont need to go the full distance of i to get somewhere
    i = i + b->maximum_move;//just to make sure that it doesnt make a move which
    //takes it below the threshold of return
    return i;
}
//returns the world size, used to calculate distance between shops
int world_size(struct bot *b){
    int i = 0;
    struct location *curr_location = b->location;
    while (curr_location != b->location || i == 0){
        curr_location = curr_location->east;
        i++;
    }
    return i;
}
//returns the distance to the fruit seller which we are to buy from
int fruit_seller (struct bot *b) {
    struct location *curr_location = b->location;
    struct location *curr_location1 = b->location;
    struct location *opt = b->location;
    struct location *opt1 = b->location;
    struct location *opt2 = b->location;
    struct location *opt3 = b->location;
    int distance = 0;
    int distance1 = 0;
    int real_distance = 0;
    int real_distance1 = 0;
    int opt_distance = 0;
    int opt_distance1 = max_distance_to_charge(b);
    int counter = 0;
    char *currfruit = b->location->fruit;
    char *currfruit1 = b->location->fruit;
    int max_move_this_turn = b->maximum_move;
    int found = FALSE;
    int optimal_price = 0;
    double optimal_price1 = 0;
    double profit = 0;
    double fake_profit = 0;
    int exit = FALSE;
    int inventory = b->fruit_kg;
    int bots;
    int bots1;
    //if a single bot world, account for place with quantity > 0
    //otherwise account for places with enough fruit to split between
    // all the bots at that location
    if (number_of_bots_single (b) == 1){
        bots = 0;
        bots1 = 0;
    }else{
        bots = number_of_bots (curr_location);
        bots1 = number_of_bots (curr_location);
    }
    while ((exit == FALSE || curr_location != b->location) && inventory == 0){
        //no need to be in this function if you have stuff in ur inventory
        if (bots != 0 && bots1 != 0){
            bots = number_of_bots (curr_location);
        }else {
            bots = 0;
        }
        if (curr_location->price < 0  && (b->cash + curr_location->price) >= 0 &&
            strcmp(curr_location->fruit, "Electricity") != 0
            // assuming everyone at the location is employing the same stratergy,
            // it is better to find a place of abundance, rather than trying to
            // buy again and again at a place with 600 fruits but alot of poeple
            // trying to buy from the place
            && curr_location->quantity > 0
            && inventory == 0){
            optimal_price = curr_location->price;
            currfruit = curr_location->fruit;
            opt = curr_location;
            curr_location1 = b->location;
            found = FALSE;
            distance = distance_buy_sell(b, opt);
            while (found == FALSE ||  curr_location1 != b->location){
                if (bots != 0 && bots1 != 0){
                    bots1 = number_of_bots (curr_location1);
                }else {
                    bots1 = 0;
                }
                if (curr_location1->price > 0 && curr_location1->quantity > bots1
                    && strcmp(curr_location1->fruit, currfruit) == 0){
                    optimal_price1 = curr_location1->price;
                    opt1 = curr_location1;
                    distance1 = distance_buy_sell(b, opt1);
                    opt_distance = optimal_distance(distance, distance1, b);
                    if (distance != 0 || distance1 != 0){
                        fake_profit = (optimal_price + optimal_price1) / opt_distance;
                        // distance can both be 0 if there is no
                        //opt price to be sold at
                    }
                    //the below conditions ensures that you do not account for
                    //shops which will take longer to reach than the game.
                    // It also stores profit for reference with other profits
                    //calculated. It also stores other info useful for debugging
                    if (fake_profit > profit && 
                        (ceil(abs(distance)) / max_move_this_turn) < (b->turns_left - 4)){
                        opt_distance1 = opt_distance;
                        real_distance = distance;
                        real_distance1 = distance1;
                        profit = fake_profit;
                        currfruit1  = currfruit;
                        opt2 = opt;
                        opt3 = opt1;
                    }
                }else if ( found == FALSE  && curr_location1 == b->location
                          && counter > 0){
                    optimal_price1 = 1;
                    //price at compost as it is the only other alternative
                }
                curr_location1 = curr_location1->east;
                counter++;
                if (curr_location1 != b->location){
                    found = TRUE;
                }
            }
        }
        curr_location = curr_location->east;
        if (curr_location != b->location){
            exit = TRUE;
        }
    }
    //calculates the distance to opt2, which is a location
    distance = distance_buy_sell(b, opt2);
    return distance;
}

//Finds the best fruit buyer of your inventory (the same output as the above
// function for fruit seller)
int fruit_buyer (struct bot *b) {
    struct location *curr_location1 = b->location;
    struct location *opt = b->location;
    struct location *opt1 = b->location;
    struct location *opt2 = b->location;
    struct location *opt3 = b->location;
    int distance = 0;
    int distance1 = 0;
    int real_distance = 0;
    int real_distance1 = 0;
    int opt_distance1 = max_distance_to_charge(b);
    int counter = 0;
    char *currfruit = b->fruit;
    char *currfruit1 = b->fruit;
    int found = FALSE;
    double optimal_price1 = 0;
    double profit = 0;
    double fake_profit = 0;
    int inventory = b->fruit_kg;
    int bots;
    if (number_of_bots_single (b) == 1){
        bots = 0;
    }else{
        bots = number_of_bots (curr_location1);
    }
    // curr_location1->quantity > (number_of_bot(b) * inventory)
    // the above conditions to to make sure that you only consider a location
    // if you+otherbots there can all sell their stuff(assuming you and other
    // bots have the same inventory, as u have the same stratergy)
    // in a single bot world this condition would be
    // curr_location1->quantity > 0
    // This is the almost the exact same inner while loop as the above,
    // the only differences being noted above
    while ((found == FALSE ||  curr_location1 != b->location) && inventory > 0){
        if (bots != 0){
            bots = number_of_bots (curr_location1);
        }else {
            bots = 0;
        }
        //find bots in the location as the
        //location moves
        // + 10 is because my bots circles between 2 points as poeple repeatedly
        // enter and leave the selling place, wasting around 5 turns.
        if (curr_location1->price > 0 && 
            curr_location1->quantity > ((bots * inventory) + 10)
            && strcmp(curr_location1->fruit, currfruit) == 0){
            optimal_price1 = curr_location1->price;
            opt1 = curr_location1;
            distance1 = distance_buy_sell(b, opt1);
            if (distance1 == 0){
                distance1 = 1;
                //Don't divide by 0 and this only occurs when you are at the
                //optimal selling location
            }
            if (abs(distance1) > 0){
                fake_profit = optimal_price1/abs(distance1);
                // distance can both be 0 if there is no opt price to be sold at
            }
            if (fake_profit > profit){
                opt_distance1 = distance1;
                real_distance = distance;
                real_distance1 = distance1;
                profit = fake_profit;
                currfruit1  = currfruit;
                opt2 = opt;
                opt3 = opt1;
            }
        }else if ( found == FALSE  && curr_location1 == b->location
                  && counter > 0){
            optimal_price1 = 1;
            //price at compost as it is the only other alternative
        }
        curr_location1 = curr_location1->east;
        counter++;
        if (curr_location1 != b->location){
            found = TRUE;
        }
    }
    distance1 = distance_buy_sell(b, opt3);
    return distance1;
}

//This function calculates the distance between two location. If both positive
//or negative it finds the absolute value and deducts the lowest value from
//the highest. If values are positive and negative, if adds the positive with
//the absolute value of the negative. It takes this and minuses it from the
//world size. Whichever value is smaller is returns.

//This is needed to be done because e.g. if a buyer is -32 distance from you
//and a seller is 33 distance from you, it will compute as a 65 distance.
//But in the real world it is a distance of 1 (due to the circulare nature)
int optimal_distance (int distance, int distance1, struct bot *b){
    int opt_distance = 0;
    int opt_distance1 = 0;
    if (abs(distance) > abs(distance1)
        && distance < 0 && distance1 < 0){
        opt_distance = abs(distance) - abs(distance1);
    }else if (abs(distance1) > abs(distance)
              && distance < 0 && distance1 < 0){
        opt_distance = abs(distance1) - abs(distance);
    }else if (abs(distance) > abs(distance1)
              && distance > 0 && distance1 > 0){
        opt_distance = abs(distance) - abs(distance1);
    }else if (abs(distance1) > abs(distance)
              && distance > 0 && distance1 > 0){
        opt_distance = abs(distance1) - abs(distance);
    }else if (abs(distance) == abs(distance1)){
        opt_distance = 2*abs(distance);
    }else if (distance > 0 && distance1 < 0){
        opt_distance = abs(distance1) + distance;
        opt_distance1 = world_size(b) - opt_distance;
        if (opt_distance > opt_distance1){
            opt_distance =  opt_distance1;
        }
    }else if (distance < 0 && distance1 > 0){
        opt_distance = abs(distance) + distance1;
        opt_distance1 = world_size(b) - opt_distance;
        if (opt_distance > opt_distance1){
            opt_distance =  opt_distance1;
        }
    }else{//this shouldn't really be executed
        opt_distance = abs(distance1) + abs(distance);
    }
    
    return abs(opt_distance);
}
//This uses the exact same function as fruit seller to return the distance
//between the buyer and seller which is used in logic. e.g. only buy an item
//if 3 rounds left if you can buy(round 1), move(round 2) and sell (round3).
//This is only possible if the distance between buyer and seller is below
// the maximum moves allowed in a turn
int optimal_profit_distance (struct bot *b) {
    struct location *curr_location = b->location;
    struct location *curr_location1 = b->location;
    struct location *opt = b->location;
    struct location *opt1 = b->location;
    struct location *opt2 = b->location;
    struct location *opt3 = b->location;
    int distance = 0;
    int distance1 = 0;
    int real_distance = 0;
    int real_distance1 = 0;
    int opt_distance = 0;
    int opt_distance1 = max_distance_to_charge(b);
    int counter = 0;
    char *currfruit = b->location->fruit;
    char *currfruit1 = b->location->fruit;
    int max_move_this_turn = b->maximum_move;
    int found = FALSE;
    int optimal_price = 0;
    double optimal_price1 = 0;
    double profit = 0;
    double fake_profit = 0;
    int exit = FALSE;
    int inventory = b->fruit_kg;
    int bots;
    int bots1;
    if (number_of_bots_single (b) == 1){
        bots = 0;
        bots1 = 0;
    }else{
        bots = number_of_bots (curr_location);
        bots1 = number_of_bots (curr_location1);
    }
    while ((exit == FALSE || curr_location != b->location) && inventory == 0){
        if (bots != 0 && bots1 != 0){
            bots = number_of_bots (curr_location);
        }else {
            bots = 0;
        }
        //no need to be in this function if you have stuff in ur inventory
        if (curr_location->price < 0 && (b->cash + curr_location->price)  >= 0
            && strcmp(curr_location->fruit, "Electricity") != 0
            && curr_location->quantity > 0
            && inventory == 0){
            optimal_price = curr_location->price;
            currfruit = curr_location->fruit;
            opt = curr_location;
            curr_location1 = b->location;
            found = FALSE;
            distance = distance_buy_sell(b, opt);
            while (found == FALSE ||  curr_location1 != b->location){
                if (bots != 0 && bots1 != 0){
                    bots1 = number_of_bots (curr_location1);
                }else {
                    bots1 = 0;
                }
                if (curr_location1->price > 0
                    && curr_location1->quantity > bots1
                    && strcmp(curr_location1->fruit, currfruit) == 0){
                    optimal_price1 = curr_location1->price;
                    opt1 = curr_location1;
                    distance1 = distance_buy_sell(b, opt1);
                    opt_distance = optimal_distance(distance, distance1, b);
                    if (distance != 0 || distance1 != 0){
                        fake_profit = ((optimal_price + optimal_price1) / opt_distance);
                        // distance can both be 0 if there is no opt
                        //price to be sold at
                        // dividing it again by distance from bot would be a
                        // great strat, but leads to confusion as other bots
                        // are trying to do the same and leading this bot to
                        // circle around for a few turns.
                    }
                    if (fake_profit > profit && 
                        (ceil(abs(distance)) / max_move_this_turn) < (b->turns_left - 4)){
                        opt_distance1 = opt_distance;
                        real_distance = distance;
                        real_distance1 = distance1;
                        profit = fake_profit;
                        currfruit1  = currfruit;
                        opt2 = opt;
                        opt3 = opt1;
                    }
                }else if ( found == FALSE  && curr_location1 == b->location
                          && counter > 0){
                    optimal_price1 = 1;
                    //price at compost as it is the only other alternative
                    
                }
                curr_location1 = curr_location1->east;
                counter++;
                if (curr_location1 != b->location){
                    found = TRUE;
                }
            }
        }
        curr_location = curr_location->east;
        if (curr_location != b->location){
            exit = TRUE;
        }
    }
    return abs(real_distance1);
}

//This calculates the quantity to be bought. By dividing the amount of bots at
//the buying location with the qquantity at the selling location
//the best buying quantity (without having extra) is claculated and used to buy
int  quantity_to_be_bought (struct bot *b) {
    struct location *curr_location = b->location;
    struct location *curr_location1 = b->location;
    struct location *opt = b->location;
    struct location *opt1 = b->location;
    struct location *opt2 = b->location;
    struct location *opt3 = b->location;
    int distance = 0;
    int distance1 = 0;
    int real_distance = 0;
    int real_distance1 = 0;
    int opt_distance = 0;
    int opt_distance1 = max_distance_to_charge(b);
    int counter = 0;
    char *currfruit = b->location->fruit;
    char *currfruit1 = b->location->fruit;
    int max_move_this_turn = b->maximum_move;
    int found = FALSE;
    int optimal_price = 0;
    double optimal_price1 = 0;
    double profit = 0;
    double fake_profit = 0;
    int exit = FALSE;
    int inventory = b->fruit_kg;
    double ret_value;
    int bots;
    int bots1;
    if (number_of_bots_single (b) == 1){
        bots = 0;
        bots1 = 0;
    }else{
        bots = number_of_bots (curr_location);
        bots1 = number_of_bots (curr_location1);
    }
    while ((exit == FALSE || curr_location != b->location) && inventory == 0){
        if (bots != 0 && bots1 != 0){
            bots = number_of_bots (curr_location);
        }else {
            bots = 0;
        }
        //no need to be in this function if you have stuff in ur inventory
        if (curr_location->price < 0 && (b->cash + curr_location->price)  >= 0 
            && strcmp(curr_location->fruit, "Electricity") != 0
            && curr_location->quantity > 0
            && inventory == 0){
            optimal_price = curr_location->price;
            currfruit = curr_location->fruit;
            opt = curr_location;
            curr_location1 = b->location;
            found = FALSE;
            distance = distance_buy_sell(b, opt);
            while (found == FALSE ||  curr_location1 != b->location){
                if (bots != 0 && bots1 != 0){
                    bots1 = number_of_bots (curr_location1);
                }else {
                    bots1 = 0;
                }
                if (curr_location1->price > 0
                    && curr_location1->quantity > bots1
                    && strcmp(curr_location1->fruit, currfruit) == 0){
                    optimal_price1 = curr_location1->price;
                    opt1 = curr_location1;
                    distance1 = distance_buy_sell(b, opt1);
                    opt_distance = optimal_distance(distance, distance1, b);
                    if (distance != 0 || distance1 != 0){
                        fake_profit = ((optimal_price + optimal_price1) / opt_distance);
                        // distance can both be 0 if there is no opt
                        //price to be sold at
                       
                    }if (fake_profit > profit && 
                        (ceil(abs(distance)) / max_move_this_turn) < (b->turns_left - 4)){
                        // above conditions makes sure that you do not go to
                        // a place which has the best profit but will take too
                        // long to reach
                        opt_distance1 = opt_distance;
                        real_distance = distance;
                        real_distance1 = distance1;
                        profit = fake_profit;
                        currfruit1  = currfruit;
                        opt2 = opt;
                        opt3 = opt1;
                    }
                }else if ( found == FALSE  && curr_location1 == b->location
                          && counter > 0){
                    optimal_price1 = 1;
                    //price at compost as it is the only other alternative
                    
                }
                curr_location1 = curr_location1->east;
                counter++;
                if (curr_location1 != b->location){
                    found = TRUE;
                }
            }
        }
        curr_location = curr_location->east;
        if (curr_location != b->location){
            exit = TRUE;
        }
    }
    // only buy the amount at the selling place, divided by how many
    // other bots are at your location (assuming that all are buying the
    // same fruit as you at the same place and selling at same location)
    // number_of_bots always 1 as you will always be at that location
    ret_value = (opt3->quantity) /  number_of_bots (b->location);
    //round down to be on the safe side
    ret_value = ceil(ret_value);
    return ret_value;
}
//returns number bots at the location
int number_of_bots (struct location *curr_location){
    int bot_counter = 0;
    struct bot_list *bots = curr_location->bots;
    while (bots != NULL){
        bot_counter++;
        bots = bots->next;
    }
    return bot_counter;
}
// returns number of bots in the whole wworld, useful for single/multibot strats
int number_of_bots_single (struct bot *b){
    int bot_counter = 0;
    int found = FALSE;
    struct location *curr_location = b->location;
    struct bot_list *bots = curr_location->bots;
    while (curr_location != b->location || found == FALSE){
        while (bots != NULL){
            bot_counter++;
            bots = bots->next;
        }
        curr_location = curr_location->east;
        found = TRUE;
    }
    return bot_counter;
}

//calculated distance to buyer/seller
int distance_buy_sell(struct bot*b, struct location *opt){
    struct location *curr_location = b->location;
    int distance = 0;
    int east_distance = 0;
    int west_distance = 0;
    if (strcmp(curr_location->name , opt->name) == 0){
        distance  = 0;
    }else {
        while (strcmp(curr_location->name , opt->name) != 0){
            curr_location = curr_location->west;
            west_distance++;
        }
        curr_location = b->location;
        while (strcmp(curr_location->name , opt->name) != 0){
            curr_location = curr_location->east;
            east_distance++;
        }
        if(west_distance <= east_distance){
            distance = west_distance * -1;
        } else {
            distance = east_distance;
        }
    }
    return distance;
}

// calculates distance to nearest compost
int nearest_compost(struct bot *b) {
    int distance = 0;
    struct location *curr_location = b->location;
    if(strcmp(b->location->fruit, "Anything") == 0
       && b->location->quantity > 0){
        distance = 0;
    }else {
        int west_distance = 0;
        int east_distance = 0;
        while(strcmp(curr_location->fruit, "Anything") != 0) {
            curr_location = curr_location->west;
            west_distance++;
        }
        curr_location = b->location;
        while(strcmp(curr_location->fruit, "Anything") != 0) {
            curr_location = curr_location->east;
            east_distance++;
        }
        if(west_distance <= east_distance){
            distance = west_distance * -1;
        } else {
            distance = east_distance;
        }
    }
    return distance;
}

// return distance to nearest electricity
// inspired by lab excercise 11 solutions
int nearest_electricity(struct bot *b) {
    int distance = 0;
    int found = FALSE;
    struct location *curr_location = b->location;
    struct location *curr_location1 = b->location;
    // If at the location, then return 0
    if(strcmp(b->location->fruit, "Electricity") == 0
       && b->location->quantity > 0){
        distance = 0;
    } else{
        // find the distance westward and eastward and whichever is the lowest
        // return that as a distance from the bot. Multiply by -1 if
        // closest distance is westward
        // This same formula of finding stuff is used everywhere in this program
        // as the main form of logic.
        int west_distance = 0;
        int east_distance = 0;
        while(found == FALSE ){
            while (strcmp(curr_location->fruit, "Electricity") != 0){
                if (curr_location == b->location && east_distance > 0){
                    found = TRUE;
                    return FALSE;
                }
                curr_location = curr_location->east;
                east_distance++;
            }if (curr_location->quantity > 0 && curr_location->price < 0){
                curr_location1 = curr_location;
                found = TRUE;
            }else if (east_distance > 0 && curr_location == b->location){
                return FALSE;
            }else{
                curr_location = curr_location->east;
                east_distance++;
            }
        }
        curr_location = b->location;
        found = FALSE;
        while(found == FALSE){
            while ( strcmp(curr_location->fruit, "Electricity") != 0){
                if (curr_location == b->location && west_distance > 0){
                    return FALSE;
                }
                curr_location = curr_location->west;
                west_distance++;
            }if (curr_location->quantity > 0 && curr_location->price < 0){
                curr_location1 = curr_location;
                found = TRUE;
            }else if (west_distance > 0 && curr_location == b->location){
                return FALSE;
            }else {
                curr_location = curr_location->west;
                west_distance++;
            }
        }if(west_distance <= east_distance){
            distance = west_distance * -1;
        } else {
            distance = east_distance;
        }
    }
    return distance;
}
