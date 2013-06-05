function [scaled] = scale2one(unscaled, Imin, Imax)

scaled = unscaled - unscaled(Imin);  % min -> 0
scaled = scaled / scaled(Imax);      % max -> 1

end