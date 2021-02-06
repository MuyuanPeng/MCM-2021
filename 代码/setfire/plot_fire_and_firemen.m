clear all;
fire=load('fire.dat');
region=load('fire_region.dat');
firemen=load('fireman.dat');

fire_centers=fire(:,1:2);
region_centers=region(:,1:2);

fire_radii=fire(:,1)*0+0.1;
region_radii=region(:,3);
firemen_radii=firemen(:,1)*0+0.1;

viscircles(fire_centers,fire_radii);
viscircles(region_centers,region_radii,'Color','b');
hold on;
plot(firemen(:,1),firemen(:,2),"o");



%%
clear all;
fire=load('C:\Users\MI\Desktop\新建文本文档.txt');
fire_centers=fire(:,1:2)*100;
fire_radii=fire(:,1)*0+1;
viscircles(fire_centers,fire_radii);
