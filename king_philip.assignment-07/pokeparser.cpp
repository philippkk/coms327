#include "pokeparser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <sstream>
#include <iomanip>
std::vector<pokemon>pokeparser::parsePokemon(){
    std::vector<pokemon> pokemon_v;
    std::ifstream file(pathThree);
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        if(lineNum < 1){
            lineNum++;
            continue;
        }
        std::stringstream ss(line);
        std::string f;
        std::vector<std::string> fields;
        int count = 0;
        int id, species_id,height,weight,base_experience,order,is_default;
        std::string identifier;
        while (std::getline(ss, f, ',')) {
           // std::cout << f << " ";
            fields.push_back(f);
            switch (count)
            {
            case 0:
                id = std::stoi(f);
                break;
            case 1:
                identifier = f;
                break;
            case 2:
             species_id= std::stoi(f);
             break;
            case 3:
                height = std::stoi(f);
                break;
            case 4:
                weight = std::stoi(f);
                break;
            case 5:
            base_experience= std::stoi(f);
                break;
            case 6:
            order= std::stoi(f);
                break;
            case 7:
            is_default= std::stoi(f);
                break;
            default:
                break;
            }
            count++;
        }
        //std::cout << std::endl;
        pokemon_v.push_back(pokemon(id,identifier,species_id,height,weight,base_experience,order,is_default));
        ss.clear(); // Clear the stringstream
    }

    file.close(); // C
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