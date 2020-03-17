# HCML
Hyper Cpp Marked Language -- Parse tagged html code to pure C++ code

Also, I explose the API to let you to create your own language tag.

## Why I create this project
Nowadays we usually use Node.js to write the web site(along with the server side), and I have to admit that Node.js is really easier for the developers to write code.

But the performance of Node.js is not quite good for some projects. It eats a lot of memory and CPU. Espically when we have to run web services on some light weight device like router, or industry control computer, we do not have enough memory or CPU resources to run a huge Node process.

Another important thing is, it's very eggache to generate HTML tag by C++ code. C++ is not designed well to do string process. But the HTML tags are all about string.

So I need a tool to parse XML-like tag into C++ code, that's what HCML does.

## Example
Look at the C++ code below:
```c++
  for ( auto const& item : node["data"] ) {
    printf("<div class=\"myLine\"><div class=\"myContent\">");
    printf("%s", item["title"]);
    printf("</div></div>");
  }
```
That's really not good for reading, and I want to kick your ass if you write code like this in some long-term program.

We may miss some end-tag or quote, and it's very pain to debug such code.

If we use HCML, we can write the same code like the following:
```html
<cxx:each>
    <cxx:var name="item" type="auto const&"></cxx:var>
    <cxx:var name="node">
        <cxx:subscript><cxx:string>data</cxx:string></cxx:subscript>
    </cxx:var>
    <cxx:block>
        <div class="myLine">
            <div class="myContent">
                <cxx:print>
                    <cxx:var name="item">
                        <cxx:subscript><cxx:string>title</cxx:string></cxx:subscript>
                    </cxx:var>
                </cxx:print>
            </div>
        </div>
    </cxx:block>
</cxx:each>
```
We combine cxx tag and HTML tag togather just as the sturcture we want. We then use hcml API to automatically generate the final C++ code. 

## API
### * Create an hcml handler 
```
hcml_t hcml_create();
```
### * Destroy an hcml handler and release all dynamic data 
```
void hcml_destroy( hcml_t h );
```
### * Get the output buffer 
```
const char * hcml_get_output( hcml_t h );
```
### * Get the last error message 
```
const char * hcml_get_errstr( hcml_t h );
```
### * Get the last error code
```
int hcml_get_errcode( hcml_t h );
```
### * Set error code and message
```
void hcml_set_error( hcml_t h, int code, const char * msgfmt, ... );
```
### * Get the output size 
```
int hcml_get_output_size( hcml_t h );
```
### * Set the static string print method
```
void hcml_set_print_method( hcml_t h, const char* method );
```
### * Get the static string print method
```
const char * hcml_get_print_method( hcml_t h );
```
### * Set the language prefix, default is "cxx" and return the old
```
void hcml_set_lang_prefix( hcml_t h, const char * prefix );
```
### * Get the language prefix
```
const char * hcml_get_lang_prefix( hcml_t h );
```
### * Change the language generator, maybe you want to write a Python generator
```
hcml_lang_generator hcml_set_lang_generator( hcml_t h, hcml_lang_generator fp );
```
### * Set and get the old extend language generator function point.
```
hcml_lang_generator hcml_set_exlang_generaotr( hcml_t h, hcml_lang_generator fp );
```
### * Dump debug structure info
```
void hcml_dump_tag( struct hcml_tag_t * root, int lv );
```
### * Parse the input file and output to a dynamically allocated memory
```
int hcml_parse( hcml_t h, const char * src_path );
```

### Extend
HCML is not only for C++ code, you can define your own language parser,.

In default, we use `cxx` as the prefix of a language tag, stands for "C++", You can set a differnt prefix by invoking `hcml_set_lang_prefix`.

If you want to write a differnet language parser or use your own tag structure, you need to implement a function defined as:
```
int my_func(hcml_node_t *, struct hcml_tag_t*, const char*);
```

### Default CXX Tags
#### cxx:string
Inner text will be output as `"text"`

#### cxx:const
Inner text will be output no changed

#### cxx:empty
Empty placehold, output nothing

#### cxx:invoke
Invoke member method on parent node, function name is in property `name`

#### cxx:call
Make a function call, function name is in property `name`

#### cxx:list
Create a init list, like `{1, 2, 3}`

#### cxx:var
Referenct to a variable in C++ code, the variable name is in property `name`, also we can use property `type` to specified the variable's type, the C++ code will be:
```
type var_name
```
And we support three other properties: `addr`, `ref`, `val`. 

* `addr` is used to get the pointer of current variable.
* `ref` is used to get the refernece of current varialbe.
* `val` is used to get the value of current pointer vairable.

#### cxx:code
Inner text will be output no changed and treated as original c++ code

#### cxx:line
Add a `;` at the end of current tag, same as property `eol` in all tag

#### cxx:print
Invoke the print method to wrap inner nodes

#### cxx:subscript
Put inner nodes inside `[]`

#### cxx:block
Put inner nodes inside a pair of `{}`

#### cxx:parentheses
Put inner nodes inside a pair of `()`

#### cxx:post_increase, cxx:pre_increase, cxx:post_decrease, cxx:pre_decrease
Operators for `a++, ++a, a--, ++a`

#### cxx:set
A Bineray operator, must has two child tag, make the first tag as left value, and the second tag as right value, like: `a = b`

#### cxx:great, cxx:greatequal, cxx:less, cxx:lessequal, cxx:equal, cxx:notequal
Binary Operators for `>, >=, <, <=, ==, !=`

#### cxx:plus, cxx:minus, cxx:times, cxx:divid, cxx:mod
Binary Operators for `+, -, *, /, %`

#### cxx:break, cxx:return, cxx:continue, cxx:true, cxx:false
Keyword for `break, return, continue, true, false`

#### cxx:new, cxx:delete
Keyword Operator for `new, delete`

#### cxx:typeinit
Create an object and invoke's constructure, the tyep info should be put in the property `type`

#### cxx:condition
An `if` code block, must contains several `cxx:case`, and no more than one `cxx:else`, a `cxx:else` must be the last child node.

#### cxx:case
`if` or `else if` depended on the position in the parent node. First child node in `cxx:case` should be the condition express

#### cxx:else
`else` keyword in `if` block

#### cxx:each
A `for( a : b )` format loop, should contains at least 3 child nodes. the first node is standed for `a`, the second node is standed for `b`, and the others will be the loop body

#### cxx:for
A `for( a; b; c )` format loop, should contains at least 4 child nodes.

#### cxx:while
A `while ( a )` format loop, should contains at least 2 child nodes. The first child node will be the condition express

#### cxx:do
A `do ... while( a )` format loop, should contains at least 2 child nodes. The first child node will be the condition express

## MIT License
MIT License

Copyright (c) 2020 Push Chen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
