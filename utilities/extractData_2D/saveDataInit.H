// Save files
string filename_wall = "extractData_wall_2D.csv";
OFstream outputFile_wall(filename_wall);
outputFile_wall.precision(12);

string filename_face = "extractData_face_2D.csv";
OFstream outputFile_face(filename_face);
outputFile_face.precision(12);

outputFile_wall << "x_center,y_1stcell,Ux_1stcell,xslope_wall" << "\n";
outputFile_face << "x_center,y_face,avgU,xslope_face,Ux_face" << "\n";
