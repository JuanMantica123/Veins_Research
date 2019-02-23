/*
 * Task.h
 *
 *  Created on: Feb 11, 2019
 *      Author: juan
 */

#ifndef SRC_RANDOMFORESTAPP_TASK_H_
#define SRC_RANDOMFORESTAPP_TASK_H_

class Task{
public:

    Task(int id,double workLoad){
        this-> workLoad = workLoad;
        this->id = id;

    }

    double getCreationTime() const {
        return creationTime;
    }

    int getId() const {
        return id;
    }

    double getProgress() const {
        return progress;
    }
    void incrementProgress(double latestProgress){
        this->progress+=latestProgress;
    }

    double getWorkLoad() const {
        return workLoad;
    }

private:
    double workLoad;
    double progress;
    double creationTime;
    int id;

};

#endif /* SRC_RANDOMFORESTAPP_TASK_H_ */
