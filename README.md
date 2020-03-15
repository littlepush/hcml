# HCML
Hyper Cpp Marked Language -- Parse tagged html code to pure C++ code

## Basic Format
Consider the code:
```html
<![cpp]>
  if ( a > b ) {
  <![html]>
    <div class="myClass">
      Text1
    </div>
  </![html]>
  } else {
  <![html]>
    <button class="myBtnClass">Click Me!</button>
  </[html]>
  }
</[cpp]>
```
Can be transfered to 
```c++
  static std::string _str1 = "<div class=\"myClass\">\nText1\n</div>";
  static std::string _str2 = "<button class=\"myBtnClass\">Click Me!</button>";
  if ( a > b ) {
    resp.write(_str1.c_str(), _str1.size());
  } else {
    resp.write(_str2.c_str(), _str2.size());
  }
```
Also, we can use tag to do some logic:
```html
<![cpp:if] cond="var_exp" assert="true">
  <div class="expTrueClass">My Text</div>
</[cpp]>
```
C++ Code should be
```c++
static std::string _str1 = "<div class=\"expTrueClass\">My Text</div>";
if ( var_ext == true ) {
  resp.write(_str1.c_str(), _str1.size());
}
```

## MIT License
To add detail
