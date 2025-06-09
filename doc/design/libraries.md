# Libraries

| Name                     | Description |
|--------------------------|-------------|
| *libkawra_cli*         | RPC client functionality used by *kawra-cli* executable |
| *libkawra_common*      | Home for common functionality shared by different executables and libraries. Similar to *libkawra_util*, but higher-level (see [Dependencies](#dependencies)). |
| *libkawra_consensus*   | Stable, backwards-compatible consensus functionality used by *libkawra_node* and *libkawra_wallet* and also exposed as a [shared library](../shared-libraries.md). |
| *libkawraconsensus*    | Shared library build of static *libkawra_consensus* library |
| *libkawra_kernel*      | Consensus engine and support library used for validation by *libkawra_node* and also exposed as a [shared library](../shared-libraries.md). |
| *libkawraqt*           | GUI functionality used by *kawra-qt* and *kawra-gui* executables |
| *libkawra_ipc*         | IPC functionality used by *kawra-node*, *kawra-wallet*, *kawra-gui* executables to communicate when [`--enable-multiprocess`](multiprocess.md) is used. |
| *libkawra_node*        | P2P and RPC server functionality used by *kawrad* and *kawra-qt* executables. |
| *libkawra_util*        | Home for common functionality shared by different executables and libraries. Similar to *libkawra_common*, but lower-level (see [Dependencies](#dependencies)). |
| *libkawra_wallet*      | Wallet functionality used by *kawrad* and *kawra-wallet* executables. |
| *libkawra_wallet_tool* | Lower-level wallet functionality used by *kawra-wallet* executable. |
| *libkawra_zmq*         | [ZeroMQ](../zmq.md) functionality used by *kawrad* and *kawra-qt* executables. |

## Conventions

- Most libraries are internal libraries and have APIs which are completely unstable! There are few or no restrictions on backwards compatibility or rules about external dependencies. Exceptions are *libkawra_consensus* and *libkawra_kernel* which have external interfaces documented at [../shared-libraries.md](../shared-libraries.md).

- Generally each library should have a corresponding source directory and namespace. Source code organization is a work in progress, so it is true that some namespaces are applied inconsistently, and if you look at [`libkawra_*_SOURCES`](../../src/Makefile.am) lists you can see that many libraries pull in files from outside their source directory. But when working with libraries, it is good to follow a consistent pattern like:

  - *libkawra_node* code lives in `src/node/` in the `node::` namespace
  - *libkawra_wallet* code lives in `src/wallet/` in the `wallet::` namespace
  - *libkawra_ipc* code lives in `src/ipc/` in the `ipc::` namespace
  - *libkawra_util* code lives in `src/util/` in the `util::` namespace
  - *libkawra_consensus* code lives in `src/consensus/` in the `Consensus::` namespace

## Dependencies

- Libraries should minimize what other libraries they depend on, and only reference symbols following the arrows shown in the dependency graph below:

<table><tr><td>

```mermaid

%%{ init : { "flowchart" : { "curve" : "basis" }}}%%

graph TD;

kawra-cli[kawra-cli]-->libkawra_cli;

kawrad[kawrad]-->libkawra_node;
kawrad[kawrad]-->libkawra_wallet;

kawra-qt[kawra-qt]-->libkawra_node;
kawra-qt[kawra-qt]-->libkawraqt;
kawra-qt[kawra-qt]-->libkawra_wallet;

kawra-wallet[kawra-wallet]-->libkawra_wallet;
kawra-wallet[kawra-wallet]-->libkawra_wallet_tool;

libkawra_cli-->libkawra_util;
libkawra_cli-->libkawra_common;

libkawra_common-->libkawra_consensus;
libkawra_common-->libkawra_util;

libkawra_kernel-->libkawra_consensus;
libkawra_kernel-->libkawra_util;

libkawra_node-->libkawra_consensus;
libkawra_node-->libkawra_kernel;
libkawra_node-->libkawra_common;
libkawra_node-->libkawra_util;

libkawraqt-->libkawra_common;
libkawraqt-->libkawra_util;

libkawra_wallet-->libkawra_common;
libkawra_wallet-->libkawra_util;

libkawra_wallet_tool-->libkawra_wallet;
libkawra_wallet_tool-->libkawra_util;

classDef bold stroke-width:2px, font-weight:bold, font-size: smaller;
class kawra-qt,kawrad,kawra-cli,kawra-wallet bold
```
</td></tr><tr><td>

**Dependency graph**. Arrows show linker symbol dependencies. *Consensus* lib depends on nothing. *Util* lib is depended on by everything. *Kernel* lib depends only on consensus and util.

</td></tr></table>

- The graph shows what _linker symbols_ (functions and variables) from each library other libraries can call and reference directly, but it is not a call graph. For example, there is no arrow connecting *libkawra_wallet* and *libkawra_node* libraries, because these libraries are intended to be modular and not depend on each other's internal implementation details. But wallet code is still able to call node code indirectly through the `interfaces::Chain` abstract class in [`interfaces/chain.h`](../../src/interfaces/chain.h) and node code calls wallet code through the `interfaces::ChainClient` and `interfaces::Chain::Notifications` abstract classes in the same file. In general, defining abstract classes in [`src/interfaces/`](../../src/interfaces/) can be a convenient way of avoiding unwanted direct dependencies or circular dependencies between libraries.

- *libkawra_consensus* should be a standalone dependency that any library can depend on, and it should not depend on any other libraries itself.

- *libkawra_util* should also be a standalone dependency that any library can depend on, and it should not depend on other internal libraries.

- *libkawra_common* should serve a similar function as *libkawra_util* and be a place for miscellaneous code used by various daemon, GUI, and CLI applications and libraries to live. It should not depend on anything other than *libkawra_util* and *libkawra_consensus*. The boundary between _util_ and _common_ is a little fuzzy but historically _util_ has been used for more generic, lower-level things like parsing hex, and _common_ has been used for kawra-specific, higher-level things like parsing base58. The difference between util and common is mostly important because *libkawra_kernel* is not supposed to depend on *libkawra_common*, only *libkawra_util*. In general, if it is ever unclear whether it is better to add code to *util* or *common*, it is probably better to add it to *common* unless it is very generically useful or useful particularly to include in the kernel.


- *libkawra_kernel* should only depend on *libkawra_util* and *libkawra_consensus*.

- The only thing that should depend on *libkawra_kernel* internally should be *libkawra_node*. GUI and wallet libraries *libkawraqt* and *libkawra_wallet* in particular should not depend on *libkawra_kernel* and the unneeded functionality it would pull in, like block validation. To the extent that GUI and wallet code need scripting and signing functionality, they should be get able it from *libkawra_consensus*, *libkawra_common*, and *libkawra_util*, instead of *libkawra_kernel*.

- GUI, node, and wallet code internal implementations should all be independent of each other, and the *libkawraqt*, *libkawra_node*, *libkawra_wallet* libraries should never reference each other's symbols. They should only call each other through [`src/interfaces/`](`../../src/interfaces/`) abstract interfaces.

## Work in progress

- Validation code is moving from *libkawra_node* to *libkawra_kernel* as part of [The libkawrakernel Project #24303](https://github.com/kawra/kawra/issues/24303)
- Source code organization is discussed in general in [Library source code organization #15732](https://github.com/kawra/kawra/issues/15732)
