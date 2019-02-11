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

    Task(double loadRemaining,int identifier,double creationTime){
        this.loadRemaining = loadRemaining;
        this.identifier = identifier;
        this.creationTime = creationTime;

    }

    double getLoadRemaining() const {
        return load;
    }

    void setLoadRemaining(double load) {
        this->load = load;
    }

    int getIdentifier() const {
        return identifier;
    }

    void setIdentifier(int identifier) {
        this->identifier = identifier;
    }

    double getCreationTime() const {
        return creationTime;
    }

    void setCreationTime(double creationTime) {
        this->creationTime = creationTime;
    }

private:
    double loadRemaining;
    int identifier;
    double creationTime;
};

#endif /* SRC_RANDOMFORESTAPP_TASK_H_ */
