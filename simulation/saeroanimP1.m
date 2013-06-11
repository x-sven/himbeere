function [sys,x0,str,ts] = saeroanim(t,x,u,flag,Config)
%SAEROANIM S-Function for displaying 6DoF trajectories
%

% Copyright 1990-2003 The MathWorks, Inc.
% J.Hodgson  
% Modified by: S.Gage
% $Revision: 1.3 $  $Date: 2003/07/18 15:18:55 $

switch flag,

  %%%%%%%%%%%%%%%%%%
  % Initialization %
  %%%%%%%%%%%%%%%%%%
  case 0,
     [sys,x0,str,ts]=mdlInitializeSizes(Config);

  %%%%%%%%%%%%%%%
  % Derivatives %
  %%%%%%%%%%%%%%%
  case {1 , 3, 9},
     sys=[];
     
  %%%%%%%%%%
  % Update %
  %%%%%%%%%%
  case 2,
     sys = [];
     if Config.Animenable
        mdlUpdate(t,x,u,Config);
     end

  %%%%%%%%%%%%%%%%%%%%%%%
  % GetTimeOfNextVarHit %
  %%%%%%%%%%%%%%%%%%%%%%%
  case 4,
    sys=mdlGetTimeOfNextVarHit(t,x,u,Config);

 
otherwise
  %%%%%%%%%%%%%%%%%%%%
  % Unexpected flags %
  %%%%%%%%%%%%%%%%%%%%

   error('aeroblks:saeroanim:invalidflag',['Unhandled flag = ',num2str(flag)]);

end

% end sanim
%
%=============================================================================
% mdlInitializeSizes
% Return the sizes, initial conditions, and sample times for the S-function.
%=============================================================================
%
function [sys,x0,str,ts]=mdlInitializeSizes(Config)
global F V
%
% Set Sizes Matrix
%
sizes = simsizes;

sizes.NumContStates  = 0;
sizes.NumDiscStates  = 0;
sizes.NumOutputs     = 0;
sizes.NumInputs      = 6;
sizes.DirFeedthrough = 0;
sizes.NumSampleTimes = 1;   % at least one sample time is needed

sys = simsizes(sizes);

%
% initialise the initial conditions
%
x0  = [];

%
% str is always an empty matrix
%
str = [];

%
% initialise the array of sample times
%
ts  = [-1 0]; % fixed sample time

if ~Config.Animenable
   return
end

%
% Initialise Figure Window
%
   h_f=findobj('type','figure','Tag','6DOF anim');
   
   if isempty(h_f)
     h_anim=figure;
   else
     h_anim=h_f;
   end

   set(h_anim,'name','Animation Figure', ...
           'renderer','z','position',[300 150 700 700], ...
           'clipping','off','Tag','6DOF anim');
   
   if ~isempty(h_anim)
      h_del = findobj(h_anim,'type','axes');
      delete(h_del);
      figure(h_anim);
   end
   


%  Initialize Axes
   
   handle.axes(1)=axes;
   axis(Config.axes); XLabel('X'); YLabel('Y'); ZLabel('Z');
   set(handle.axes(1),'visible','on','xtick',[],'ytick',[],'ztick',[],'box','on', ...
           'CameraPosition',Config.camera_pos,'dataaspectratio',[1 1 1], ...
           'projection','pers', ...
           'units','normal', ...
           'position',[0.1 0.1 0.75 0.75], ...
           'Color',[1 1 1], ...
           'drawmode','fast', ...
           'clipping','off');


     % Initialize Trajectory 
%
 
   handle.line(1) = line(0,0,0);
   set(handle.line(1),'linestyle','-','color',[1 1 1],'erasemode','nor','userdata',0,'clipping','off');
   handle.line(2) = line(0,0,0);
   set(handle.line(2),'linestyle',':','color',[1 1 1],'erasemode','nor','clipping','off');
      

% Draw in Target Position
%   

	%P.Vert = Config.craft/2*[-1 -1 -1;1 -1 -1;1 1 -1;-1 1 -1;-1 -1 1;1 -1 1;1 1 1;-1 1 1];
    P.Vert = V;
	%P.Vert(:,1)=P.Vert(:,1)+Config.target(1);
	%P.Vert(:,2)=P.Vert(:,2)+Config.target(2);
	%P.Vert(:,3)=P.Vert(:,3)+Config.target(3);
	%P.faces = [1 2 6 5;2 3 7 6;3 4 8 7;4 1 5 8;1 2 3 4;5 6 7 8];
    P.faces = F;
	handle.target = patch('vertices',P.Vert,'faces',P.faces);
	set(handle.target,'facecolor',[1 0 0], ...
                  'edgecolor',[0 0 0]);
    vert = get(handle.target,'vertices');
    set(handle.axes(1),'userdata',vert)           

   set(h_anim,'userdata',handle);


%=============================================================================
% mdlUpdate
% Handle discrete state updates, sample time hits, and major time step
% requirements.
%=============================================================================
%
function mdlUpdate(t,x,u,Config)

 
% Obtain Handles of Figure Objects
%
    handle = get(findobj('type','figure','Tag','6DOF anim'),'userdata');

    if isempty(findobj('type','figure','Tag','6DOF anim'))
     %figure has been manually closed
     return
    end

% Form Transformation Matrix
%
    cph = cos(u(4));		% Roll
    sph = sin(u(4));
    cth = cos(u(5));		% Pitch
    sth = sin(u(5));
    cps = cos(u(6));		% Yaw
    sps = sin(u(6)); 
    attitude = [cth*cps sph*sth*cps-cph*sps cph*sth*cps+sph*sps
                cth*sps sph*sth*sps+cph*cps cph*sth*sps-sph*cps
                -sth         sph*cth         cph*cth];
             
      
%
% Update Craft Object 
%
   vert = get(handle.axes(1),'userdata');
   a=size(vert,1);
   dum =attitude*vert'+repmat(u(1:3,1),1,a);
   set(handle.target,'vertices',dum');

% Update Line Objects
%
    x1 = get(handle.line(1),'Xdata');  
    x2 = get(handle.line(1),'Ydata');
    x3 = get(handle.line(1),'Zdata');
    init = get(handle.line(1),'userdata');
    if init
       x1 = [x1 u(1)];
       x2 = [x2 u(2)];
       x3 = [x3 u(3)];
    else
       x1 = u(1);
       x2 = u(2);
       x3 = u(3);
       set(handle.line(1),'userdata',1);
    end

    set(handle.line(1),'Xdata',x1, ...
                       'Ydata',x2, ...
                       'Zdata',x3);	
                   
    set(handle.line(2),'Xdata',x1, ...
                       'Ydata',x2, ...
       		       'Zdata',repmat(Config.axes(6),length(x1),1));	
%
% Set position of target view to Target

%switch Config.camera_view 
   
%   case 1,			% Fixed Observer Position
   %set(handle.axes(1),'cameraupvector',[0 0 -1], ...
    %  'cameraposition',Config.camera_pos, ...
     % 'cameratarget',u(1:3), ...
      %'cameraviewangle',Config.view);
  %case 2,			% Cockpit View
%   Target= Config.target';
  % ax = Config.axes;
  % seeker_dir = sqrt(sum((ax(2)-ax(1))^2+(ax(4)-ax(3))^2+(ax(6)-ax(5))^2))*attitude*[1;0;0];
   %seeker_pos = attitude*[Config.craft 0 0]';
  % set(handle.axes(1),'cameraupvector',attitude*[0 0 -1]', ...
      %     'cameraposition',u(1:3)+seeker_pos, ...
        %   'cameratarget',u(1:3)+seeker_dir, ...
        %   'cameraviewangle',Config.view);
        %case 3,			% Relative Position View
   %set(handle.axes(1),'cameraupvector',[0 0 -1], ...
    %       'cameraposition',u(1:3)+Config.camera_pos', ...
     %      'cameratarget',u(1:3), ...
      %     'cameraviewangle',Config.view);
end

%
% Force MATLAB to Update Drawing
%
   drawnow

% end mdlUpdate


%=============================================================================
% mdlGetTimeOfNextVarHit
% Return the time of the next hit for this block.  
%=============================================================================
%
function sys=mdlGetTimeOfNextVarHit(t,x,u,Config)
    
    sys = ceil(t/Config.update)*Config.update+Config.update;

% end mdlGetTimeOfNextVarHit

%
