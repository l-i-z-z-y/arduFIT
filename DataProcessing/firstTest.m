
clc
clear all
close all

'load files from '
folderPath = 'C:/Users/Lukas/Documents/Sensorik/TestData/SmartPhoneExamples1/*.txt'
sampling_frequnecy = 10;


files = glob(folderPath)
for i=1:numel(files)
  [~, name] = fileparts (files{i});
  ## eval(sprintf('%s = load("%s", "-ascii");', name, files{i}));
  
  S = load(files{i});
  figure()
  subplot(2,1,1);
  hold on
  t = S(:,1)/10;
  plot(t, S(:,2));
  plot(t, S(:,3));
  plot(t, S(:,4));
  title(files{i});
  xlabel('time')
  ylabel('m/s^2')
  
  subplot(2,1,2);
  N = length(t);
  for j=1:N
    f(j) = (j-1) * sampling_frequnecy/N;    
  endfor
  
  hold on
  y=fft(S(:,2));
  plot(f,abs(y))
  y=fft(S(:,3));
  plot(f,abs(y))
  y=fft(S(:,4));
  plot(f,abs(y))

endfor