;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;														;
;	Linked List Example.											;
;														;
;	This example shows how to add/remove/search through nodes in a singly (normal) linked			;
;	list.													;
;														;
;	A normal linked list only has a pointer to the next node. The data structure looks like this:		;
;														;
;	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;								;
;	;	Address of Next Node		;								;
;	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;								;
;	;	Data[0]				;								;
;	;	Data[1]				;								;
;	;	Data[2]				;								;
;	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;								;
;														;
;	* Note that we are using a doubly linked list in Lab 3 so you should not be copy and pasting		;
;	this code.												;
;														;
;	The program will ask for a node name with a length of 3 characters. When the user types those 3		;
;	characters, the program will first search through the linked list and if not found, then		;
;	it will add the new node into the linked list. If a match was found, the program should remove		;
;	that node from the linked list.										;
;														;
;	*Inserting*												;
;	The way we create a new node is to just	increment the last node's address by 0x0010. For example,	;
;	if the current last node is at 0x4300, then the new node should be located at 0x4310.			;
;														;
;	*Removing*												;
;	When the user types in an existing node name, the program will remove that node from the list.		;
;														;
;	Let's assume the first node of the linked list should be 0x4000. Thus, the next nodes should be		;
;	0x4010, 0x4020, etc. We will assume that the linked list is originally empty.				;
;														;
;	Each data value can be any ASCII character.								;
;														;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


		.ORIG x3000

PROMPT		LEA R0, MSG_PROMPT	; display prompt
		PUTS

		; Create a new node. Remember the new node location should be the last node's address + 0x10
		LD R1, LAST_NODE	; go to the last node
		BRz INIT		; if there's not a last node, we need to initialize our linked list with a first node at 0x4000
		ADD R1, R1, #8		; if there is a last node, add 0x10 (16) for the new node location
		ADD R1, R1, #8
		BR INIT_DONE

		; Set our current node pointer to 0x4000
INIT		LD R1, FIRST_NODE_INIT	; R1 = 0x4000
		BR INIT_DONE

INIT_DONE	ADD R1, R1, #1		; set R1 to the pointer to the beginning of the data of the node
		AND R2, R2, #0		; R2 is our counter, set to 3 because ID's are 3 characters long
		ADD R2, R2, #3

LOOP_PROMPT	GETC
		OUT
		STR R0, R1, #0		; store into saved ID location
		ADD R1, R1, #1		; increment pointer
		ADD R2, R2, #-1		; decrement counter
		BRp LOOP_PROMPT

		LD R0, NEWLINE		; print a newline
		OUT
		ADD R0, R1, #-4		; set R0 to the beginning of the node
		JSR SEARCH
		ADD R6, R6, #0		; see if the node was found (R6 should be 1 if node was found)
		BRp CALL_REMOVE		; found node, so remove from list
		BR CALL_INSERT		; not found, so insert the node

CALL_INSERT	JSR INSERT
		BR OPERATION_DONE

CALL_REMOVE	JSR REMOVE
		BR OPERATION_DONE

OPERATION_DONE	AND R0, R0, #0		; we need to reset PREVIOUS_NODE
		ST R0, PREVIOUS_NODE
		BR DISPLAY

		; Print out current state of linked list.
DISPLAY		LD R0, NEWLINE
		OUT
		LEA R0, MSG_LIST
		PUTS
		LD R1, FIRST_NODE	; R1 is our pointer to the current node
		BRz DISPLAY_DONE
		AND R6, R6, #0		; R6 counts the number of nodes
		ADD R6, R6, #1
		LD R5, NUMBER_OFFSET

DISPLAY_LOOP	LEA R0, MSG_NODE	; print node number
		PUTS
		AND R0, R0, #0
		ADD R0, R6, R5		; get ASCII value of node number
		OUT
		LD R0, NEWLINE
		OUT
		LEA R0, MSG_ID		; print node ID
		PUTS
		LDR R0, R1, #1
		OUT
		LDR R0, R1, #2
		OUT
		LDR R0, R1, #3
		OUT
		LD R0, NEWLINE
		OUT
		OUT
		LDR R2, R1, #0		; R2 = M[R1]
		BRz DISPLAY_DONE
		ADD R1, R2, #0		; go to next node
		ADD R6, R6, #1		; increment counter
		BR DISPLAY_LOOP
		
DISPLAY_DONE	LD R0, NEWLINE
		OUT
		BR PROMPT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Our subroutine for search. The subroutine assumes that:							;
;	- R0 contains the address of the node to search for. We only need its data, not its actual address.	;
;	If the node is found, set R6 to 1. If node is not found, set R6 to 0.					;
;	At the end of search:											;
;	- R0 contains the address of the found node								;
;	- R6 contains 1 if the node was found, 0 if not.							;
;	- PREVIOUS_NODE contains the address of the previous node. This will be used later in remove.		;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

SEARCH		ST R0, SAVE_R0		; we'll be reloading R0 after each iteration of the loop
		ST R2, SAVE_R2
		ST R3, SAVE_R3
		ST R4, SAVE_R4
		ST R7, SAVE_R7		; we need to save R7 because we call TRAP in our subroutine
		
		AND R6, R6, #0		; default is not found
		LD R2, FIRST_NODE	; check if there's a first node
		BRz SEARCH_DONE

		; Traverse the linked list.
		; To make it easy on ourselves,
		; - R2 will be the pointer to the current node
		; - R1 will be the pointer to the previous node. Note that we set R1 at the end before we move on to the next node

		; Now we need to compare the data between the current node and the node in R0
TRAVERSE	AND R5, R5, #0		; R5 is our counter, set it to 3 because we will be comparing 3 times
		ADD R5, R5, #3
		LD R0, SAVE_R0		; reload R0
		ST R2, SAVE_R2		; save the current node's address

COMPARE		ADD R2, R2, #1		; R2 now points to the data of current node
		ADD R0, R0, #1		; R0 now points to the data of node we're searching for
		LDR R1, R2, #0		; R1 = M[R2]
		LDR R4, R0, #0		; R4 = M[R0]
		NOT R4, R4		; compare R1 and R4
		ADD R4, R4, #1
		ADD R4, R1, R4
		BRnp NOT_EQUAL		; stop comparing immediately if not found
		ADD R5, R5, #-1		; decrement the counter
		BRz EQUAL		; if counter reaches 0, then we have a match
		BR COMPARE

NOT_EQUAL	LD R2, SAVE_R2		; reset R2 to beginning of node
		LDR R3, R2, #0		; R3 = address of next node
		BRz SEARCH_DONE
		ADD R1, R2, #0		; R1 = saved address of current node
		ST R1, PREVIOUS_NODE
		ADD R2, R3, #0		; R2 = address of the next node
		BR TRAVERSE

		; Save the found node if we found a match.
		;
		; If we didn't find a match, then the value in SAVE_R0 is correct since we need to add the node to the linked list.
		;
		; However, if we found a match, then the value in SAVE_RO is incorrect because R0 is a newly created node.
		; If we found a match then R0 should be set to the found node's value, which is in R2.
		;
		; So, in essence:
		; - If no match, then SAVER0 should stay as the newly created node
		; - If match, then SAVER0 should contain the existing node (R2) to be removed
EQUAL		ADD R2, R2, #-3		; reset R0 back to the address, not the data
		ST R2, SAVE_R0		; save found node
		ADD R6, R6, #1		; set flag
		BR SEARCH_DONE		; we are done searching if there's a match
		
SEARCH_DONE	ADD R6, R6, #0		; print found/not found according to R6
		BRz PRINT_NOT_FOUND
		LEA R0, MSG_FOUND
		PUTS
		BR SEARCH_RETURN

PRINT_NOT_FOUND	LEA R0, MSG_NOT_FOUND
		PUTS
		BR SEARCH_RETURN
		
SEARCH_RETURN	LD R0, SAVE_R0
		LD R2, SAVE_R2
		LD R3, SAVE_R3
		LD R4, SAVE_R4
		LD R7, SAVE_R7		; our original return address
		RET

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Our subroutine for insert. We only need to insert to the end of the linked list.			;
;	The subroutine assumes that:										;
;	- R0 contains the address of the node to insert.							;
;	We'll use R1 as the pointer to the last node of the list.						;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

INSERT		ST R1, SAVE_R1		; we don't need to save R0 since we won't change it
		LD R1, LAST_NODE	; check if there's a last node
		BRz INSERT_FIRST	; insert into the beginning
		STR R0, R1, #0		; set the next node to the address of the node we're inserting
		ST R0, LAST_NODE
		BR INSERT_DONE

		; Create the first node
INSERT_FIRST	ST R0, FIRST_NODE	; first node now points to us
		ST R0, LAST_NODE	; last node is also the first node
		BR INSERT_DONE

INSERT_DONE	LD R1, SAVE_R1
		RET
		
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;	Our subroutine for remove. There are 4 cases to consider:						;
;														;
;	1. Removing from the end.										;
;	If the node we're removing is the last node (its next pointer is null), then all we need to do		;
;	is set the previous node's next pointer to null.							;
;														;
;	2. Removing from the middle of 2 nodes.									;
;	If the node we're removing is in between 2 nodes, then we need to set the previous node's next pointer	;
;	to our next pointer.											;
;														;
;	3. Removing from the beginning.										;
;	If the node we're removing is the first node, then we just need to set the label to the first node to	;
;	our next pointer.											;
;														;
;	4. Removing the only node.										;
;	This case will happen from case 3. If the node had no previous and has no next, then we set both	;
;	FIRST_NODE and LAST_NODE to null									;
;														;
;	To remove, our subroutine assumes that:									;
;	- R0 contains the address of the node to remove								;
;	- PREVIOUS_NODE contains the address of the previous node (if it exists)				;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

REMOVE		ST R1, SAVE_R1
		ST R2, SAVE_R2
		
		LD R1, PREVIOUS_NODE	; test the condition code
		BRz REMOVE_CASE_3	; there is no previous node, so we are in Case 3
		LDR R2, R0, #0		; R2 = pointer to our next node
		BRz REMOVE_CASE_1	; there is no next node, so we are in Case 1
		BR REMOVE_CASE_2	; there is a previous and next node, so we are in Case 2

		; Case 1 - Set the previous node's next pointer to null
REMOVE_CASE_1	AND R2, R2, #0		; set R2 = 0
		STR R2, R1, #0		; set previous node's next pointer to null
		ST R1, LAST_NODE	; our last node is now the previous node
		BR REMOVE_DONE

		; Case 2 - Set previous node's next pointer to our next pointer
REMOVE_CASE_2	LDR R2, R0, #0		; R2 = pointer to our next node
		STR R2, R1, #0		; set previous node's next pointer to our next pointer
		BR REMOVE_DONE

		; Case 3 - Set the FIRST_NODE label to our next pointer.
		; Note that we have to also check for the case that we are the only node. In that case, move to Case 4.
REMOVE_CASE_3	LDR R2, R0, #0		; R2 = pointer to our next node
		BRz REMOVE_CASE_4
		ST R2, FIRST_NODE	; first node is now the next node
		BR REMOVE_DONE

		; Case 4 - Reset the labels FIRST_NODE and LAST_NODE
REMOVE_CASE_4	AND R0, R0, #0
		ST R0, FIRST_NODE
		ST R0, LAST_NODE
		BR REMOVE_DONE

REMOVE_DONE	AND R1, R1, #0		; clear our pointer
		STR R1, R0, #0

		LD R1, SAVE_R1
		LD R2, SAVE_R2
		RET

NEWLINE		.FILL 0x000A	; ASCII code for newline
NUMBER_OFFSET	.FILL 0x0030	; offset from number to ASCII value

FIRST_NODE	.FILL 0x0000	; address of the first node
LAST_NODE	.FILL 0x0000	; address of last node
PREVIOUS_NODE	.FILL 0x0000	; address of previous node (used in remove)
FIRST_NODE_INIT	.FILL 0x4000	; if first node doesn't exist, initialize at 0x4000

MSG_PROMPT	.STRINGZ "Please enter a node ID:\t"
MSG_NOT_FOUND	.STRINGZ "Node not in list. Adding node to list...\n"
MSG_FOUND	.STRINGZ "Node in list. Removing node from list...\n"
MSG_LIST	.STRINGZ "Current linked list state:\n"
MSG_NODE	.STRINGZ "Node "
MSG_ID		.STRINGZ "ID:\t"

SAVE_R0		.BLKW 1
SAVE_R1		.BLKW 1
SAVE_R2		.BLKW 1
SAVE_R3		.BLKW 1
SAVE_R4		.BLKW 1
SAVE_R7		.BLKW 1

		.END