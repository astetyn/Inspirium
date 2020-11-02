#ifndef TASK_LIST_H
#define TASK_LIST_H

struct TaskNode {

    enum {
        TYPE_TIME,
        TYPE_DISTANCE
    } type;

    int motor1 = 127;
    int motor2 = 127;

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

template <typename T> struct FuncNode {

    T func;
    FuncNode<T> *next;

    FuncNode(const T &f, FuncNode<T> *n) : func(f), next(n) {}

};

template <typename T> class EventList {

    public:

        EventList() {
            head = 0;
            tail = 0;
            lsize = 0;
        }

        void registerCallback(T const &func) {

            FuncNode<T> *node = new FuncNode<T>(func, 0);

            if(head == 0) {
                head = node;
            }else {
                tail->next = node;
            }

            tail = node;

            lsize++;

        }

        void unregisterCallback(T const &func) {

            FuncNode<T> *node = head;

            for(int i = 0; i < lsize; i++) {

                if(node->next == 0) {
                    return;
                }

                if(node->next->func == func) {
                    delete node->next;
                    node->next = node->next->next;
                    lsize--;
                    return;
                }
            }

        }

        T getAt(int index) {

            FuncNode<T> *node = head;

            while(node != 0 && index > 0) {
                
                node = node->next;
                index--;

            }

            if(node == 0) {
                return 0;
            }

            return node->func;
        }

        int size(void) {return lsize;}

    private:

        FuncNode<T> *head, *tail;
        int lsize;

};

#endif