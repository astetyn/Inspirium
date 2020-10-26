#include "TaskList.h"

using namespace std;

TaskList::TaskList() {
    head = 0;
    tail = 0;
}

void TaskList::add(TaskNode *node) {

    node->next = 0;

    if(head == 0) {
        head = node;
    }else {
        tail->next = node;
    }

    tail = node;

}

TaskNode *TaskList::front() {

    return head;

}

void TaskList::pop() {

    if(head == 0) {
        return;
    }

    TaskNode *temp = head;

    head = head->next;

    delete temp;

}

void TaskList::clear() {

    while(head != 0) {
        pop();
    }

}