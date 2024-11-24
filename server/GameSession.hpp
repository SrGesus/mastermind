#ifndef GAMESESSION_HPP_
#define GAMESESSION_HPP_

#include <time.h>
#include <cstdlib>
#include <common/Trial.hpp>
#include <common/utils.hpp>

class GameSession{
    public:
        int _plid, _time, _nT, _score, _startTime;
        bool _playing;
        Trial _code;
        Trial _trials[8];
    
    enum TrialResult{
        WIN = 1,
        PLAYING = 0,
        INV = -3,
        TIMEOUT = -2,
        LOSS = -1
    };

    GameSession() : _plid(0), _time(0), _nT(0), _score(0), _startTime(0), _playing(false) {
        srand(time(NULL));
    }

    GameSession(int plid, int maxTime, char code[8]){
        _plid = plid;
        _time = maxTime;
        _nT = 0;
        _score = 0;
        _playing = true;
        _startTime = time(NULL);
        _code = Trial(code, 7);
        srand(time(NULL));
    }

    GameSession(int plid, int maxTime){
        _plid = plid;
        _time = maxTime;
        _nT = 0;
        _score = 0;
        _startTime = time(NULL);
        _playing = true;
        char code[8];
        generateCode(code);
        _code = Trial(code, 7);
        srand(time(NULL));

    }

    void generateCode(char *code) {
        for (int i = 0; i < 7; i+=2) {
        switch (rand() % 6){
            case 0:
            code[i] = Color::red;
            break;
            case 1:
            code[i] = Color::green;
            break;
            case 2:
            code[i] = Color::blue;
            break;
            case 3:
            code[i] = Color::yellow;
            break;
            case 4:
            code[i] = Color::orange;
            break;
            case 5:
            code[i] = Color::purple;
            break;
        }
        }
        code[7] = '\0';
    }

    /// @brief Executes a trial
    /// @param trial 
    /// @param nT integer between 0 and 7 representing the trial number sent by the client
    /// @return result of the attempt
    int executeTrial(Trial trial, int nT){
        bool result = trial.evaluateNumbers(_code);

        // check if trial has been played before
        for(int i = 0; i <= _nT; i++){
            if(strcmp(trial.getTrial(), _trials[i].getTrial()) == 0){
                return PLAYING;
            }
        }

        // check if trial is invalid
        if (nT<_nT || (nT = _nT && strcmp(trial.getTrial(), _trials[nT].getTrial()) != 0)){
            return INV;
        }

        // check if there is still time left
        if(time(NULL) - _startTime > _time){
            _playing = false;
            return TIMEOUT;
        }

        _trials[_nT] = trial;
        _nT++;
        if(result){
            _playing = false;
            /// @todo calculate score
            _score = 100 - (time(NULL) - _startTime);
            return WIN; // win condition
        }
        if (_nT == 8){
            _playing = false;
            return LOSS; // loss condition
        }
        return PLAYING; // trial was played
    }
    
    void endGame(){
        _playing = false;
    }
};

#endif  // GAMESESSION_HPP_