David Bui
CIS 277
HW #8
Picking Via Raycasting

**************Mouse Controls:**************

Click (left mouse button) + drag: 	Move cube
Mouse wheel: 						Zoom in/out


Click on a face of the cube to select that face.  From here, you can apply the face operations below (change color, split into triangles).  The point where you clicked will be visible as a white line.  You can see this line if you hold down the mouse button and drag around the window.

BONUS:  Hovering
	You can move the mouse cursor over a face on the cube, and that face will light up to indicate that it is underneath the cursor.

************** MESH OPERATIONS **************

FACES:

Splitting a Face
	To split a face, simply select it from the Faces list and click "Split Face".  Note that the face must have exactly 4 edges, otherwise it will not be split.

Changing Face Color
	To change a face's color, select it from the Faces list, input the new RGB values into the appropriate widgets (marked Red, Green, and Blue) and apply your changes using the "Apply Color" button.

EDGES:

Adding a Vertex
	To add a new vertex at the midpoint of the currently selected edge, click "Add Vertex".

VERTICES

Adding an Edge
	To add a new edge, select a face from the Faces list, and select a vertex from the Vertices list.  Next, click "Add Edge" to create the edge. *Note that in order for this to work, there must not be at least one face that shares the currently selected vertex, but not an edge!* If this is not fulfilled, the button will not do anything, no matter how many times you click it.  You will have to split faces in the cube to get this to work.

	Quick test: Split Face 0, select Face 1 and Vertex 0, and click "Add Edge".

Deleting a Vertex
	To delete a vertex, select one from the Vertices list, and click "Delete Vertex".  Deleting a vertex also deletes any faces that contain the vertex.

Moving a Vertex
	To move a vertex, input the amount you would like to displace it by in the boxes marked X, Y, Z and click "Apply Position".



Note: 
I chose to comment out the code that highlights the active vertex and active half-edge pink and cyan, respectively, because this assignment focuses more on selecting faces. This functionality is easy to get back by uncommenting the commented-out lines in the traverse() function.