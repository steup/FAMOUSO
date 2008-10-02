function simview;

% ************* Create simulator window **************
		h = findobj('Tag','SimWindow');
      delete(h);
      h = findobj('Tag','EditorWindow');
      delete(h);

		figNumber = figure;

      ssize = get(0,'ScreenSize');
      set(figNumber,	'MenuBar','none',...
         				'CloseRequestFcn','simviewcb close',...
         				'Position',[1 ssize(4)*0.13 ssize(3)/2 ssize(4)*0.8],...
				         'NumberTitle','off',...
				         'Name','Simulation Window',...
				         'RendererMode','manual',...
				         'Renderer','Painters',...
				         'Backingstore','off',...
				         'Tag','SimWindow');
		axis equal
		axHndl=gca;
	  	color = get(figNumber,'Color'); 
  
		set(axHndl, ...
     					   'XLim',[0 200],'YLim',[0 200], ...
							'XDir','normal','YDir','normal', ...
							'Drawmode','fast', ...
							'Visible','on', ...
							'Tag','SimAxes',...
							'TickLength',[0 0],...
							'XColor',color,...
							'YColor',color);
		h1 = uicontrol('Parent',figNumber, ...
							'Units','points', ...
							'BackgroundColor',[0.8 0.8 0.8], ...
							'FontName','Symbol', ...
							'FontSize',7, ...
							'ForegroundColor',[0.7 0.7 0.7], ...
							'ListboxTop',0, ...
							'Position',[ssize(3)-(220*ssize(3)/800) 6 10 10], ...
							'String','x', ...
							'Style','text', ...
						   'Tag','ListStore');                  
	   h1 = uicontrol('Parent',figNumber, ...
							'Units','points', ...
							'BackgroundColor',[0.8 0.8 0.8], ...
							'FontName','Symbol', ...
							'FontSize',7, ...
							'ForegroundColor',[0.7 0.7 0.7], ...
							'ListboxTop',0, ...
							'Position',[20 6 10 10], ...
							'String','x', ...
							'Style','text', ...
                     'Tag','SimPath',...
                     'Visible','off');                  
		h1 = uicontrol('Parent',figNumber, ...
							'Units','points', ...
							'BackgroundColor',[0.8 0.8 0.8], ...
							'FontName','Symbol', ...
							'FontSize',7, ...
							'ForegroundColor',[0.7 0.7 0.7], ...
							'ListboxTop',0, ...
							'Position',[20 6 10 10], ...
							'String','x', ...
							'Style','text', ...
                     'Tag','RunType',...
                     'Visible','off');                                    
		h1 = uicontrol('Parent',figNumber, ...
							'Units','points', ...
							'BackgroundColor',[0.8 0.8 0.8], ...
							'FontName','Symbol', ...
							'FontSize',7, ...
							'ForegroundColor',[0.7 0.7 0.7], ...
							'ListboxTop',0, ...
							'Position',[20 6 10 10], ...
							'String','x', ...
							'Style','text', ...
                     'Tag','ActualStep',...
                     'UserData',0,...
                     'Visible','off');
		h1 = uicontrol('Parent',figNumber, ...
							'Units','points', ...
							'BackgroundColor',[0.8 0.8 0.8], ...
							'FontName','Symbol', ...
							'FontSize',7, ...
							'ForegroundColor',[0.7 0.7 0.7], ...
							'ListboxTop',0, ...
							'Position',[ssize(3)-(220*ssize(3)/800) 6 10 10], ...
							'String','x', ...
							'Style','text', ...
                     'Tag','SimMatrix',...
                     'Visible','off');                                    
   	h1 = uicontrol('Parent',figNumber, ...
							'Units','points', ...
							'BackgroundColor',[0.752941176470588 0.752941176470588 0.752941176470588], ...
							'ListboxTop',0, ...
							'Max',0, ...
							'Position',[ssize(3)/2-250 10*ssize(3)/800 34 21], ...
							'Style','frame', ...
							'Tag','Frame1');

		h1 = uicontrol('Parent',figNumber, ...
							'Units','points', ...
						   'BackgroundColor',[0.752941176470588 0.752941176470588 0.752941176470588], ...
						   'FontWeight','bold',...
							'ListboxTop',0, ...
							'Position',[ssize(3)/2-250 10*ssize(3)/800 32 10], ...
							'String','Step', ...
							'Style','text', ...
                     'Tag','StepText');
                  
                  
      % ****** ContextMenu ******
		uicm = uicontextmenu;
      set(axHndl,'UIContextMenu',uicm);
      	uimenu(uicm,'Label','&Open ..','Callback','simviewcb load');
         uimenu(uicm,'Label','&Save ..','Callback','simviewcb save','Enable','off',...
            			'Tag','SaveMenu');
         uimenu(uicm,'Label','&Edit','Callback','simviewcb editor','Enable','off',...
                     'Tag','EditMenu','Separator','on');
         paruicm = 	uimenu(uicm,'Label','&Parameters','Tag','ParamsMenu',...
            				'Callback','simviewcb params','Enable','off',...
				            'Separator','on');
							uimenu('Parent',paruicm,'Label','&Steps limit: Inf ','Tag','StepsMenu',...
         		 			'Callback','simviewcb steps','Enable','off');
      					uimenu('Parent',paruicm,'Label','Step &time: 0.001 ','Tag','DelayMenu',...
	         	   		'Callback','simviewcb delay','Enable','off');
         
         uimenu(uicm,'Label','&Draw tracks','Callback','simviewcb tracks',...
                     'Tag','TracksMenu','Enable','off','Separator','on');
                  
%			uimenu(uicm,'Label','&Jump to step 0','Callback','simviewcb rewind',...
%                     'Enable','off','Separator','on','Tag','RewMenu');                        
			uimenu(uicm,'Label','&Run simulation','Callback','simviewcb sim',...
                     'Enable','off','Separator','on','Tag','RunMenu');      
			uimenu(uicm,'Label','Run repl&ay','Callback','simviewcb rep',...
                     'Enable','off','Tag','RunRepMenu');      
			uimenu(uicm,'Label','S&top','Callback','simviewcb stop',...
                     'Enable','off','Separator','on','Tag','StopContMenu');      
             
      
      % ****** Window Menu ******
      filemenu = uimenu('Label','&File');
      fmenu1 = uimenu(	'Parent',filemenu,'Label','&Open ..','Callback','simviewcb load',...
         					'Accelerator','O');
      			uimenu(	'Parent',filemenu,'Label','&Save ..','Callback','simviewcb save',...
					         'Enable','off','Accelerator','S','Tag','SaveMenu');         
      			uimenu(	'Parent',filemenu,'Label','&Edit','Callback','simviewcb editor',...
         					'Enable','off','Tag','EditMenu','Accelerator','E','Separator','on');
		      
      			uimenu(	'Parent',filemenu,'Label','&Quit','Callback','simviewcb close',...
      					   'Accelerator','Q','Separator','on');
   
	   parmenu = 	uimenu('Label','&Parameters','Enable','off','Tag','ParMenu');
   		   		uimenu('Parent',parmenu,'Label','&Steps limit: Inf ','Tag','StepsMenu',...
         		 			 'Callback','simviewcb steps','Enable','off');
      				uimenu('Parent',parmenu,'Label','Step &time: 0.001 ','Tag','DelayMenu',...
         	   			 'Callback','simviewcb delay','Enable','off');

               
		runmenu = uimenu(	'Label','&Run','Enable','off','Tag','WinRunMenu');     
                uimenu(	'Parent',runmenu,'Label','&Draw tracks','Accelerator','D',...
                		   'Callback','simviewcb tracks','Tag','TracksMenu','Enable','off');
                uimenu(	'Parent',runmenu,'Label','&Run simulation','Accelerator','R',...
                   		'Callback','simviewcb sim','Tag','RunMenu','Enable','off',...
		                  'Separator','on');
                uimenu(	'Parent',runmenu,'Label','Run repl&ay','Accelerator','A',...
                   		'Callback','simviewcb rep','Tag','RunRepMenu','Enable','off');
                uimenu(	'Parent',runmenu,'Label','S&top','Accelerator','T',...
                   		'Callback','simviewcb stop','Tag','StopContMenu','Enable','off',...
                   		'Separator','on');

     
                     
		% *************************
		simviewcb initialize
% **************************************************************

