uint32 tt;

int GetTimes(){
  
  if (rt.getTime()!=tt)
  {
    tt = rt.getTime();
    
    return (int)tt;
  }
  
}

String ParseTime(int seconds){
  int s = seconds, m = 0, h = 0; //s=36000
 
  String str = "";
  int time=s;
  if (time/60/60<10) { str += ("0"); }
  str += (time/60/60);
  str += (":");
  if (time/60%60<10) { str += ("0"); }
  str += ((time/60)%60);
  str += (":");
  if (time%60<10) { str += ("0"); }
  str += (time%60);

  return str;
}

int GetMinutes(){
  return GetTimes() / 60;
}

int SetMinutes(int seconds){
  return seconds * 60;
}

int OstToTimers(int timer[]){
  return GetTimes() - timer[0];
}

bool IsTimer(int timer[]){
  if(GetTimes() - timer[0] > timer[1]){
    timer[0] = GetTimes();
    return true;
  }
  else{
    return false;
  }
}

