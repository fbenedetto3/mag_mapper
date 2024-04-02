function [] = pathGeneratorCompile(x,y,z,xres,yres,zres, xCenter, yCenter, zCenter)
%%Generates a csv file with magnetic field mapper gird. 
%Inputting gridGenerator(x,y,z,xres,yres,zres) will output a path with
%resolution being specified for every cardinal direction. 
%Inputting gridGenerator(x, y, z, xres, yres, zres, xCenter, yCenter, zCenter)
%will output a grid with with shifted coordinates around a specified center. This is useful when a
%a scan needs to surround a specific sample. Running the 
%function with no variables will prompt the function to ask user to input
%xyz scan size and resolution.
if ischar(x)
    x = str2double(x);
    y = str2double(y);
    z = str2double(z);
    xres = str2double(xres);
    yres = str2double(yres);
    zres = str2double(zres);
end

for i = 1 %This is so we can ask the user if they want to use this grid configuration



xSize = x/xres;
ySize = y/yres;
zSize = z/zres;  %A cool function would be to add variable resolution for each layer, or even variable resolution as you get farther from point



if(rem(xSize,1) ~=0) && xres ~=0
    a = num2str(xSize);
    error(['x/resolution is not an integer!', a])
end
if(rem(ySize,1) ~=0)  && yres ~=0
    error('y/resolution is not an integer!')
end
if(rem(zSize,1) ~=0)  && zres ~=0
    error('z/resolution is not an integer!')
end

if xres == 0
    xres = 1;
    xSize = 0;
end
if yres == 0
    yres = 1;
    ySize = 0;
end
if zres == 0
    zres = 1;
    zSize = 0;
end

%Inner is all the coordinates the path must go to to complete the scan. For
%a 2D scan over a grid of size nxn,
%the number of movements to go over each point will be n^2 + 2n. 
%On a 3D grid of size nxnxm, you will have n^2 + 2n number of moves 
%for each layer, plus 1 corresponding to each layer you move up. The number
%of times you have to move up is m-1, so the amount of moves will end up
%being (m+1)(n^2+2n)+m+1. Expanding further, the number of movements for a grid
%of size nxlxm would be equal to m(nl+(j+n))+m-1. Add one if we want to
%include the starting position as a "movement"

%Thus inner will be a grid of size m(nl+(l+n))+m-1.:3, to move to
%everypoint

innerSize = (xSize + 1)*(zSize+1) * (ySize+1);

%Time estimate is a guess right now. Would have to come up with a function
%that estimates how long it takes to move between points with some
%resolution value and add the amount of time it takes to scan a single
%point.
% timeEstimate = innerSize*5/1000 *(1/60);
% ask = sprintf('Are you sure you want to continue. Estimated time to scan with current grid setting is %.1f hours and %.1f minutes.', floor(timeEstimate/60), mod(timeEstimate,60));
% disp(ask);

%%your code here
% m=input('Do you want to continue, Y/N [Y]:','s');
% if m=='N'
% break
% end


inner = zeros(innerSize+1,3);



n = 2;
xEnd = x;
yEnd = y;

lowerZ = zCenter;
upperZ = zCenter + z;
upperY = yCenter + y/2;
upperX = x/2 + xCenter;
lowerY = yCenter-y/2;
lowerX = xCenter - x/2;

inner(1,:) = [lowerX, lowerY, lowerZ];


for zCord = lowerZ:zres:upperZ
    if inner(n-1,2) == lowerY
        for yCord = lowerY:yres:upperY
            if inner(n-1,1) == lowerX
                for xCord = lowerX:xres:upperX
                    inner(n,:) = [xCord, yCord, zCord];
                    n=n+1;
                end
            else
                    for xCord = upperX:-xres:lowerX
                    inner(n,:) = [xCord, yCord, zCord];
                    n=n+1;
                    end
            end
        end

    else 
        for yCord = upperY:-yres:lowerY
            if inner(n-1,1) == lowerX
                for xCord = lowerX:xres:upperX
                    inner(n,:) = [xCord, yCord, zCord];
                    n=n+1;
                end
            else
                    for xCord = upperX:-xres:lowerX
                    inner(n,:) = [xCord, yCord, zCord];
                    n=n+1;
                    end
            end
        end
        
    end
end

%%This is to shift the path. If no shift variable inputted, function
%%automatically centers the path in the x and y plane of the printbed
%%This is assuming a printbed of 235 mm x 235 mm.

    x_coords = inner(:, 1);
    x_coords(1) = [];
    y_coords = inner(:, 2);
    y_coords(1) = [];
    z_coords = inner(:, 3);
    z_coords(1) = [];

end

inner = [x_coords, y_coords, z_coords];
T = table(x_coords,y_coords,z_coords);
writetable(T,'coordinates.csv','WriteVariableNames',0);
end
