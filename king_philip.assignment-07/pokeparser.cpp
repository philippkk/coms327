#include "pokeparser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <sstream>
#include <iomanip>
std::vector<pokemon>pokeparser::parsePokemon(){
    std::vector<pokemon> pokemon_v;
    std::ifstream file(pathOne);
    std::string line;

    if(!file.is_open()){
        std::cout<<"path 1 not open"<<std::endl;
        file.open(pathTwo);
        if(!file.is_open()){
            std::cout<<"path 2 not open"<<std::endl;
            file.open(pathThree);
            if(!file.is_open()){
            std::cout<<"path 3 not open"<<std::endl;
            return pokemon_v;
        }
        }
    }
    int lineNum = 0;
    while (std::getline(file, line)) {
        if(lineNum < 1){
            lineNum++;
            continue;
        }
        std::stringstream ss(line);
        std::string f;
        int count = 0;
        int id, species_id,height,weight,base_experience,order,is_default;
        std::string identifier;
        while (std::getline(ss, f, ',')) {
            switch (count)
            {
            case 0:
                try{
                    id = std::stoi(f);
                }catch(const std::invalid_argument& e){
                    id = INT_MAX;
                }
                break;
            case 1:
                identifier = f;
                break;
            case 2:
             try{
                    species_id = std::stoi(f);
                }catch(const std::invalid_argument& e){
                    species_id = INT_MAX;
                }
             break;
            case 3:
                try{
                    height = std::stoi(f);
                }catch(const std::invalid_argument& e){
                    height = INT_MAX;
                }
                break;
            case 4:
                try{
                    weight = std::stoi(f);
                }catch(const std::invalid_argument& e){
                    weight = INT_MAX;
                }
                break;
            case 5:
            try{
                    base_experience = std::stoi(f);
                }catch(const std::invalid_argument& e){
                    base_experience = INT_MAX;
                }
                break;
            case 6:
            try{
                    order = std::stoi(f);
                }catch(const std::invalid_argument& e){
                    order = INT_MAX;
                }
                break;
            case 7:
            try{
                    is_default = std::stoi(f);
                }catch(const std::invalid_argument& e){
                    is_default = INT_MAX;
                }
                break;
            default:
                break;
            }
            count++;
        }
        pokemon_v.push_back(pokemon(id,identifier,species_id,height,weight,base_experience,order,is_default));
        ss.clear(); 
    }
    file.close();
    return pokemon_v;
};

pokeparser::pokeparser(){ 
    std::string home_dir = getenv("HOME");

    pathOne = "/share/cs327/pokedex/pokedex/data/csv/";
    pathTwo =  home_dir + "/.poke327/pokedex/pokedex/data/csv/";
    pathThree = home_dir +"/Desktop/pokedex/pokedex/data/csv/pokemon.csv";
}
pokeparser::~pokeparser(){ }

pokemon::pokemon(int iD, std::string i, int sid,int h, int w, int be, int o, int isd){
    id = iD;
    identifier = i;
    species_id = sid;
    height = h;
    weight = w;
    base_experience = be;
    order = o;
    is_default = isd;
}
pokemon::pokemon(){}