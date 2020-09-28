#ifndef TASK_LIST_H
#define TASK_LIST_H

struct TaskNode {

    enum {
        TYPE_TIME,
        TYPE_DISTANCE
    } type;

    char motor1 = 127;
    char motor2 = 127;

    TaskNode *next;

};

struct TimeTask : TaskNode {

    TimeTask() {
        type = TYPE_TIME;
    }

    int millis;

};

struct DistanceTask : TaskNode {

    DistanceTask() {
        type = TYPE_DISTANCE;
    }

    int centimetres;

};

class TaskList {

    public:

        TaskList();
        void add(TaskNode *taskNode);
        TaskNode *front();
        void pop();
        void clear();

    private:

        TaskNode *head, *tail;

};

#endif