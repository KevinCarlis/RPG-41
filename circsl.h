#pragma once
#include "Bridges.h"
#include "CircSLelement.h"
#include "actor.h"
#include <memory>
using namespace bridges;

CircSLelement<shared_ptr<Actor>>* insert_actor(CircSLelement<shared_ptr<Actor>> *head, shared_ptr<Actor> a) {
	if (!head)
		head = new CircSLelement<shared_ptr<Actor>> (a, a->name);
	else {
		CircSLelement<shared_ptr<Actor>> *curr = head;
		CircSLelement<shared_ptr<Actor>> *in = new CircSLelement<shared_ptr<Actor>> (a, a->name);
		if (a->speed > head->getValue()->speed) {
			while (true) {
				if (curr->getNext() == head) {
					curr->setNext(in);
					break;
				}
				curr = curr->getNext();
			}
			in->setNext(head);
			head = in;
		}
		else {
			while (true) {
				if (curr->getNext() == head) {
					curr->setNext(in);
					in->setNext(head);
					break;
				}
				else if (a->speed > curr->getNext()->getValue()->speed) {
					in->setNext(curr->getNext());
					curr->setNext(in);
					break;
				}
				else 
					curr = curr->getNext();
			}
		}
	}
	return head;
}

CircSLelement<shared_ptr<Actor>>* remove_actor(CircSLelement<shared_ptr<Actor>> *head, shared_ptr<Actor> remove) {
	if (!head)
		return head;
	CircSLelement<shared_ptr<Actor>> *prev = head;
	CircSLelement<shared_ptr<Actor>> *curr = head->getNext();
	while(curr->getValue() != remove) {
		prev = curr;
		curr = curr->getNext();
	}
	if (curr == head)
		head = curr->getNext();
	prev->setNext(curr->getNext());
	return head;
}
