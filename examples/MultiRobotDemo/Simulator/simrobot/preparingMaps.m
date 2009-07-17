function preparingMaps()
global scenario;

fprintf('Loading map ... %s',scenario.bmp_name)
matrix_bmp=imread(scenario.bmp_name,'bmp');
matrix=ones(size(matrix_bmp,1),size(matrix_bmp,2));
[row,column]=find(matrix_bmp(:,:,1)==255 & matrix_bmp(:,:,2)==255 & matrix_bmp(:,:,3)==255);
for i=1:length(row)
    matrix(row(i),column(i))=0;
end
matrix=rot90(matrix,3);
% Filterung nach den Kantenübergängen
laplace=[[0 1 0];[1 -4 1];[0 1 0]];
contourline=conv2(laplace,matrix);
matrix=contourline(2:size(matrix,1)+1,2:size(matrix,2)+1);
[x_p y_p]=find(matrix==1 | matrix==2 );
scenario.matrix=zeros(size(matrix));
for i=1:length(x_p)
    scenario.matrix(x_p(i),y_p(i))=1;
end
fprintf('                      [ok]\n');
end