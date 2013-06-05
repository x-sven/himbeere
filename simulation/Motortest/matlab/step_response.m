function [time_vec, rpm_cmd_vec, rpm_state_vec] = step_response(data_struct)

signals = importdata(data_struct.file);


[Y, MO, D, H, M, S] = datevec(signals.textdata(3:end,1));
signals.Time = (60*60 *H + 60 *M +S)-(60*60 *H(1) + 60 *M(1) +S(1)) ; 


rpm_cmd = (signals.data(:,14 -1));

rpm_cmd = scale2one(rpm_cmd, 16* data_struct.T_min_rpm, 16* data_struct.T_max_rpm);

% rpm_cmd =  rpm_cmd/  ((rpm_cmd(16* data_struct.T_max_rpm) - rpm_cmd(16* data_struct.T_min_rpm)) / ...
%                       ( signals.data(16*data_struct.T_max_rpm ,9 -1)- signals.data(16*data_struct.T_min_rpm ,9 -1) )) ;
% rpm_cmd =  rpm_cmd-  (rpm_cmd(16* data_struct.T_min_rpm) - signals.data(16* data_struct.T_min_rpm ,9 -1));
% 


index_vec = 16* data_struct.T_switch-16*1:16* data_struct.T_switch+16*1;

time_vec = signals.Time(index_vec);
rpm_cmd_vec  = rpm_cmd (index_vec);

rpm_state_vec  = scale2one(signals.data(:,9 -1), 16* data_struct.T_min_rpm, 16* data_struct.T_max_rpm);
rpm_state_vec = rpm_state_vec(index_vec);

% plot(signals.Time(index_vec), signals.data(index_vec,9 -1), 'r.-')
% hold on
% 
% plot(time_vec, rpm_cmd_vec, 'b--')
% ylabel(signals.textdata(2,9));
% 
% grid on
% hold off

% Pow = signals.data(:,3-1) .* signals.data(:,4-1);
% plot(signals.Time, Pow, 'r.-') 
% ylabel(signals.textdata(2,6));

end