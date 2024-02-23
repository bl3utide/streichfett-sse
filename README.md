# ImGui Project (for Visual Studio)
C++ Project template using SDL2 and ImGui in Visual Studio

## Dependencies (Latest version)
- [SDL 2.0.10](https://github.com/libsdl-org/SDL/tree/release-2.0.10)
- [ImGui 1.87 w/ docking](https://github.com/ocornut/imgui/tree/1ee252772ae9c0a971d06257bb5c89f628fa696a)
- [plog 1.1.10](https://github.com/SergiusTheBest/plog/tree/1.1.10)

## Changelog

### v0.2.2
- 依存するライブラリをサブモジュールとして追加
- メインプロジェクトのプロジェクト名を`App`に変更
- メインプロジェクトのソースファイルおよびヘッダーファイルをsnake_caseに統一
  - ソリューションファイル(.sln)、プロジェクトファイル(.vcxproj)等のファイル名はこれまで通りUpperCamelCaseとする

### v0.2.1
- メインプロジェクトのsrcをインクルードディレクトリとして追加
- 同じカテゴリのソースファイルをサブディレクトリに移動
- 出力される実行ファイルをソリューション名に変更(デフォルトはImGuiProject.exe)
- GUIのカラー定義用のソースファイルを追加

### v0.2.0
- 依存するライブラリをソリューションディレクトリに格納
- アプリケーションのベース名前空間の名称をソリューション名(リポジトリ名)と統一
- メインのプロジェクトの名前をApplicationに変更
- StateモジュールとErrorモジュールをMainモジュールから独立

### v0.1.1
- SDLライブラリをソースからコンパイルするように変更
