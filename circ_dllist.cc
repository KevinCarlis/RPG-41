/**
 * Created by Kalpathi Subramanian, 2/20/18
 * krs@uncc.edu
 */

#include "Bridges.h"
#include "CircDLelement.h"
#include "actor.h"


using namespace bridges;

// helper function
CircDLelement<Actor> *insertFront(
	CircDLelement<Actor> *tailElement,
	CircDLelement<Actor> *newElement);

int main(int argc, char **argv) {
    Bridges *bridges =  new Bridges(999, "YOUR_USER_ID",
                                "YOUR_API_KEY");

	bridges->setTitle("An Example Circular Doubly Linked List");

	// create the linked list elements with
	// student data
	Actor(string userName, int userSpeed, unsigned int userAttack, unsigned int userHP, int userColor) {
	CircDLelement<Actor> *students[] = {
		new CircDLelement<Actor>(
			Actor(
				"Gretel Chaney",
				12,
				1,
				132,
				2
			), ""),
		new CircDLelement<Actor>(
			Actor(
				"Karol Soderman",
				11,
				23,
				143,
				1
			), ""),
		new CircDLelement<Actor>(
			Actor(
				"Lamont Kyler",
				23,
				12,
				95,
				2
			), ""),
		new CircDLelement<Actor>(
			Actor(
				"Gladys Serino",
				17,
				22,
				113,
				6
			), ""),
		new CircDLelement<Actor>(
			Actor(
				"Starr Mcginn",
				25,
				16,
				420,
				5
			), "")
	};

	CircDLelement<Actor> *head =  nullptr;

	int num_students = 5;
	for (int i = 0; i < num_students; i++) {
		if (i)
			head = insertFront(head, students[i]);
		else
			head = students[i];
	}

	//  add visual attributes
	CircDLelement<Actor> *current = head;
	StudentInfo si;
	do {
		si = current->getValue();
		current->setLabel(si.GetName());
		current->getVisualizer()->setColor(si.GetColor());

		current->getLinkVisualizer(current->getNext())->setColor(si.GetColor());
		current->getLinkVisualizer(current->getNext())->setThickness(si.GetSpeed()*.5);

		current->getLinkVisualizer(current->getPrev())->setColor(si.GetColor());
		current->getLinkVisualizer(current->getPrev())->setThickness(si.GetSpeed()*.5);

		current = current->getNext();
	}  while (current != head);

	// set data structure to point to head
	bridges->setDataStructure(head);
	// visualize the circular list
	bridges->visualize();

	return 0;
}

CircDLelement<Actor> *insertFront(
	CircDLelement<Actor> *tailElement,
	CircDLelement<Actor> *newElement) {

	CircDLelement<Actor> *tailNextElement = tailElement->getNext();

	newElement->setNext(tailNextElement);
	newElement->setPrev(tailElement);

	tailNextElement->setPrev(newElement);
	tailElement->setNext(newElement);

	return tailElement;
}
